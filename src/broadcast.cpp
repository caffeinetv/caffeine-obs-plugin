#include "broadcast.hpp"

#include <chrono>
#include <ctime>
#include <random>
#include <thread>
#include <filesystem>

// To generate random UUID
#include <QUuid>

#include "rest-api.hpp"
#include "policy.hpp"

using namespace std::chrono_literals;

namespace caff {
Broadcast::Broadcast(SharedCredentials &sharedCredentials, std::string username,
		     std::string title,
		     std::optional<std::vector<std::string>> hashtags,
		     std::optional<std::string> category)
	: sharedCredentials_(sharedCredentials),
	  clientId_(QUuid::createUuid().toString().mid(1, 36).toStdString()),
	  username_(std::move(username)),
	  title_(std::move(title)),
	  thumbnailFileName_(""),
	  contentRating_(caffql::ContentRating::Unknown),
	  feedId_(QUuid::createUuid().toString().mid(1, 36).toStdString()),
	  hashtags_(hashtags),
	  category_(category)
{
}

char const *
Broadcast::broadcastStateString(Broadcast::BroadcastState broadcastState)
{
	switch (broadcastState) {
	case BroadcastState::Offline:
		return "Offline";
	case BroadcastState::Starting:
		return "Starting";
	case BroadcastState::Streaming:
		return "Streaming";
	case BroadcastState::Live:
		return "Live";
	case BroadcastState::Stopping:
		return "Stopping";
	default:
		return "Unknown";
	}
}

bool Broadcast::requireBroadcastState(
	BroadcastState expectedBroadcastState) const
{
	BroadcastState currentBroadcastState = broadcastState_;
	if (currentBroadcastState != expectedBroadcastState) {
		CAFF_LOG_ERROR("In broadcast state %s when expecting %s",
			       broadcastStateString(currentBroadcastState),
			       broadcastStateString(expectedBroadcastState));
		return false;
	}
	return true;
}

bool Broadcast::transitionBroadcastState(BroadcastState expectedBroadcastState,
					 BroadcastState newBroadcastState)
{
	BroadcastState oldState = expectedBroadcastState;
	bool result = broadcastState_.compare_exchange_strong(
		oldState, newBroadcastState);
	if (!result)
		CAFF_LOG_ERROR(
			"Transitioning to state %s expects state %s but was in state %s",
			broadcastStateString(newBroadcastState),
			broadcastStateString(expectedBroadcastState),
			broadcastStateString(oldState));
	return result;
}

bool Broadcast::isOnline() const
{
	switch (broadcastState_) {
	case BroadcastState::Offline:
	case BroadcastState::Starting:
	case BroadcastState::Stopping:
		return false;
	case BroadcastState::Streaming:
	case BroadcastState::Live:
		return true;
	default:
		return false;
	}
}
std::variant<std::string, SessionAuthResponse> Broadcast::createSrtFeed()
{
	CAFF_LOG_DEBUG("Creating feed: %s", feedId_.c_str());
	auto feed = currentFeedInput();

	auto payload = graphqlRequest<caffql::Mutation::AddFeedField>(
		sharedCredentials_, clientId_, caffql::ClientType::Obsplugin,
		feed);
	if (!payload) {
		CAFF_LOG_ERROR("Request failed adding feed");
		return BroadcastFailed;
	}

	if (payload->error) {
		if (std::holds_alternative<caffql::OutOfCapacityError>(
			    payload->error->implementation)) {
			return OutOfCapacity;
		} else {
			CAFF_LOG_ERROR("Error adding feed: %s",
				       payload->error->message().c_str());
			return BroadcastFailed;
		}
	}

	if (payload->stage.feeds.empty() ||
	    payload->stage.feeds[0].id != feedId_) {
		CAFF_LOG_ERROR("Expected feed in AddFeedPayload");
		return BroadcastFailed;
	}

	if (!payload->stage.broadcastId) {
		CAFF_LOG_ERROR("Expected broadcast id in AddFeedPayload");
		return BroadcastFailed;
	}

	broadcastId_ = payload->stage.broadcastId;

	auto stream = std::get_if<caffql::SrtStream>(
		&payload->feed.stream.implementation);
	if (!stream) {
		CAFF_LOG_ERROR("Expected srt stream in AddFeedPayload");
		return BroadcastFailed;
	}

	std::vector<caffql::SrtConnection> srtConnections =
		stream->srtConnection;
	// Active connection type heartbeat url
	heartbeatUrl_ = srtConnections[0].heartbeatUrl;
	snapshotUrl_ = srtConnections[0].snapshotUrl;
	CAFF_LOG_DEBUG("Feed snapshot url %s",
		       srtConnections[0].snapshotUrl.c_str());

	streamUrl_ = srtConnections[0].srtUrl;
	CAFF_LOG_DEBUG("Feed srt stream url %s", streamUrl_.c_str());
	return stream->srtConnection[0].srtUrl;
}

void Broadcast::start(std::function<void()> startedCallback,
		      std::function<void(SessionAuthResponse)> failedCallback)
{
	this->failedCallback_ = failedCallback;
	transitionBroadcastState(BroadcastState::Offline,
				 BroadcastState::Starting);
	broadcastThread_ = std::thread([=] {
		setupSubscription();

		if (!transitionBroadcastState(BroadcastState::Starting,
					      BroadcastState::Streaming)) {
			return;
		}
		startHeartbeat(startedCallback);
	});
}

bool Broadcast::setTitle(std::string title)
{
	bool titleChanged = false;
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (title != this->title_) {
			this->title_ = title;
			titleChanged = true;
		}
	}

	if (titleChanged && broadcastState_ == BroadcastState::Live) {
		return updateTitle();
	}
	return true;
}

bool Broadcast::setContentRating(caffql::ContentRating contentRating)
{
	bool contentRatingChanged = false;
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (contentRating != this->contentRating_) {
			this->contentRating_ = contentRating;
			contentRatingChanged = true;
		}
	}

	if (contentRatingChanged && broadcastState_ == BroadcastState::Live) {
		return updateContentRating();
	}
	return true;
}

bool Broadcast::setThumbnailImage(std::string filename)
{
	bool thumbnailChanged = false;
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (filename != this->thumbnailFileName_) {
			this->thumbnailFileName_ = filename;
			thumbnailChanged = true;
		}
	}

	if (thumbnailChanged && broadcastState_ == BroadcastState::Live) {
		return updateSnapshotFile(broadcastId_.value(), filename,
					  sharedCredentials_);
	}
	return true;
}

std::vector<uint8_t> Broadcast::createSnapshot()
{
	try {
		// Get snapshot
		std::vector<uint8_t> snapshotImageData =
			getPreviewImage(snapshotUrl_, sharedCredentials_);
		return snapshotImageData;
	} catch (...) {
		CAFF_LOG_ERROR(
			"Caught exception when trying to fetch snapshot image");
		failedCallback_(BroadcastFailed);
		return {};
	}
}

void Broadcast::startHeartbeat(std::function<void()> startedCallback)
{
	if (!requireBroadcastState(BroadcastState::Streaming)) {
		return;
	}

	CAFF_LOG_DEBUG("Awaiting stage subscription");
	// Make sure the subscription has opened before going live
	{
		auto openedFuture = subscriptionOpened_.get_future();
		auto status = openedFuture.wait_for(5s);
		if (status != std::future_status::ready) {
			CAFF_LOG_ERROR(
				"Timed out waiting for stage subscription open");
			failedCallback_(
				SessionAuthResponse::StageSubscriptionTimeOut);
			return;
		}
	}

	// Set stage live
	CAFF_LOG_DEBUG("Setting stage live");

	auto startPayload =
		graphqlRequest<caffql::Mutation::StartBroadcastField>(
			sharedCredentials_, clientId_,
			caffql::ClientType::Obsplugin, fullTitle(),
			currentSocialMetadataInput());
	if (!startPayload || startPayload->error || !startPayload->stage.live) {
		if (startPayload) {
			CAFF_LOG_ERROR("Error starting broadcast: %s",
				       startPayload->error->message().c_str());
		}
		failedCallback_(SessionAuthResponse::BroadcastFailed);
		return;
	}

	if (!transitionBroadcastState(BroadcastState::Streaming,
				      BroadcastState::Live)) {
		graphqlRequest<caffql::Mutation::StopBroadcastField>(
			sharedCredentials_, clientId_, std::nullopt);
		CAFF_LOG_ERROR("Caffeine stopping broadcast %s",
			       broadcastStateString(broadcastState_));
		return;
	}
	startedCallback();

	// Wait for the application to provide the first frame to fetch and upload snapshot
	std::thread([&]() {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		// Verify if file exists
		if (!thumbnailFileName_.empty() &&
		    std::filesystem::exists(thumbnailFileName_)) {
			if (!updateSnapshotFile(broadcastId_.value(),
						thumbnailFileName_,
						sharedCredentials_)) {
				CAFF_LOG_ERROR(
					"Failed to upload custom thumbnail");
			}
		} else {
			std::vector<uint8_t> snapshotImageData =
				createSnapshot();
			if (!updateSnapshot(broadcastId_.value(),
					    snapshotImageData,
					    sharedCredentials_)) {
				CAFF_LOG_ERROR("Failed to send screenshot");
			}
		}
	}).detach();

	auto constexpr heartbeatInterval = 5000ms;
	auto constexpr checkInterval = 100ms;

	auto interval = heartbeatInterval;

	int const max_failures = 5;
	int failures = 0;

	CAFF_LOG_DEBUG("Starting heartbeats");
	for (; broadcastState_ == BroadcastState::Live;
	     std::this_thread::sleep_for(checkInterval)) {
		interval += checkInterval;
		if (interval < heartbeatInterval)
			continue;

		interval = 0ms;

		// Heartbeat broadcast
		auto heartbeatResponse =
			heartbeatStream(heartbeatUrl_, sharedCredentials_);

		if (heartbeatResponse) {
			CAFF_LOG_DEBUG("Heartbeat succeeded");
			failures = 0;
		} else {
			CAFF_LOG_ERROR("Heartbeat failed");
			++failures;
			if (failures > max_failures) {
				CAFF_LOG_ERROR(
					"Heartbeat failed %d times; ending broadcast.",
					failures);
				failedCallback_(
					SessionAuthResponse::Disconnected);
				return;
			}
		}
	}

	if (broadcastState_ == BroadcastState::Stopping) {
		graphqlRequest<caffql::Mutation::StopBroadcastField>(
			sharedCredentials_, clientId_, std::nullopt);
	}
}

void Broadcast::stop()
{
	broadcastState_ = BroadcastState::Stopping;
	subscription_ = nullptr;
	try {
		if (broadcastThread_.joinable()) {
			std::thread([&]() {
				// Async join so we don't block this thread
				broadcastThread_.join();
			}).detach();
		}
	} catch (std::system_error ex) {
		CAFF_LOG_ERROR("Caught system error when stopping broadcast %s",
			       ex.what());
	} catch (...) {
		CAFF_LOG_ERROR("Caught exception when stopping broadcast");
	}
	broadcastState_ = BroadcastState::Offline;
}

ConnectionQuality Broadcast::getConnectionQuality()
{
	std::lock_guard<std::mutex> lock(mutex);
	return connectionQuality_;
}

caffql::FeedInput Broadcast::currentFeedInput()
{
	std::lock_guard<std::mutex> lock(mutex);
	caffql::FeedInput feed{};
	feed.id = feedId_;
	feed.game = caffql::GameInput{};
	feed.srtConnectionTypes = {caffql::SrtConnectionType::Active};

	switch (connectionQuality_) {
	case ConnectionQualityGood:
		feed.sourceConnectionQuality =
			caffql::SourceConnectionQuality::Good;
		break;
	case ConnectionQualityPoor:
		feed.sourceConnectionQuality =
			caffql::SourceConnectionQuality::Poor;
		break;
	case ConnectionQualityUnknown:
		break;
	}
	return feed;
}

caffql::SocialMetadataInput Broadcast::currentSocialMetadataInput()
{
	std::lock_guard<std::mutex> lock(mutex);
	caffql::SocialMetadataInput metadataInput{};
	metadataInput.hashtags = hashtags_;
	metadataInput.category = category_;
	metadataInput.inReactionTo = caffql::RefSocialActivity{};

	return metadataInput;
}

std::string Broadcast::fullTitle()
{
	std::lock_guard<std::mutex> lock(mutex);
	return annotateTitle(title_);
}

bool Broadcast::updateFeed()
{
	auto feed = currentFeedInput();
	auto payload = graphqlRequest<caffql::Mutation::UpdateFeedField>(
		sharedCredentials_, clientId_, caffql::ClientType::Obsplugin,
		feed);
	if (payload && payload->error) {
		CAFF_LOG_ERROR("Error updating feed: %s",
			       payload->error->message().c_str());
	}
	return payload && !payload->error;
}

bool Broadcast::updateTitle()
{
	auto payload = graphqlRequest<caffql::Mutation::ChangeStageTitleField>(
		sharedCredentials_, clientId_, caffql::ClientType::Obsplugin,
		fullTitle());
	if (payload && payload->error) {
		CAFF_LOG_ERROR("Error updating title: %s",
			       payload->error->message().c_str());
	}
	return payload && !payload->error;
}

bool Broadcast::updateContentRating()
{
	auto payload =
		graphqlRequest<caffql::Mutation::ChangeStageContentRatingField>(
			sharedCredentials_, clientId_,
			caffql::ClientType::Obsplugin, contentRating_);
	if (payload && payload->error) {
		CAFF_LOG_ERROR("Error updating rating: %s",
			       payload->error->message().c_str());
	}
	return payload && !payload->error;
}

void Broadcast::setupSubscription(size_t tryNum)
{
	std::weak_ptr<Broadcast> weakThis = shared_from_this();

	CAFF_LOG_DEBUG("Setting up GraphQL subscription. Attempt %zu", tryNum);

	auto messageHandler = [weakThis](caffql::GraphqlResponse<
					 caffql::StageSubscriptionPayload>
						 update) mutable {
		CAFF_LOG_DEBUG("Subscription message received");
		auto strongThis = weakThis.lock();
		if (!strongThis) {
			CAFF_LOG_INFO("Broadcast no longer exists. Ignoring.");
			return;
		}

		if (auto payload =
			    std::get_if<caffql::StageSubscriptionPayload>(
				    &update)) {
			CAFF_LOG_DEBUG("Stage update received");
			if (strongThis->subscriptionState_ ==
			    SubscriptionState::None) {
				strongThis->subscriptionState_ =
					SubscriptionState::Open;
				strongThis->subscriptionOpened_.set_value(true);
			}

			auto const &feeds = payload->stage.feeds;
			auto feedIt = std::find_if(
				feeds.begin(), feeds.end(),
				[&](auto const &feed) {
					return feed.id == strongThis->feedId_;
				});
			bool feedIsOnStage = feedIt != feeds.end();

			if (feedIsOnStage) {
				if (strongThis->subscriptionState_ ==
				    SubscriptionState::Open) {
					CAFF_LOG_DEBUG(
						"Feed has appeared on stage");
					strongThis->subscriptionState_ =
						SubscriptionState::FeedHasAppeared;
				}

				if (payload->stage.live) {
					CAFF_LOG_INFO("Stage has gone live");
					strongThis->subscriptionState_ =
						SubscriptionState::
							StageHasGoneLive;
				}
			} else if ((strongThis->subscriptionState_ ==
					    SubscriptionState::FeedHasAppeared ||
				    strongThis->subscriptionState_ ==
					    SubscriptionState::StageHasGoneLive) &&
				   strongThis->isOnline()) {
				// If our feed has appeared in the subscription and is no longer there
				// while we think we're online, then we've failed.
				CAFF_LOG_ERROR(
					"Feed no longer exists on stage");
				strongThis->failedCallback_(
					SessionAuthResponse::Takeover);
				return;
			} else {
				CAFF_LOG_ERROR("Feed not present in stage");
			}

			if (!payload->stage.live &&
			    strongThis->subscriptionState_ ==
				    SubscriptionState::StageHasGoneLive &&
			    strongThis->broadcastState_ ==
				    BroadcastState::Live) {
				// If the stage is no longer live after we've gone live with our feed, then we've failed.
				CAFF_LOG_ERROR("Stage failed to go live");
				strongThis->failedCallback_(
					SessionAuthResponse::Takeover);
			}
		} else if (auto errors =
				   std::get_if<std::vector<caffql::GraphqlError>>(
					   &update)) {
			// TODO: See if we can report more meaningful errors to user than "broadcast failed"
			CAFF_LOG_ERROR(
				"Error(s) creating GraphQL subscription:");
			for (auto &error : *errors) {
				CAFF_LOG_ERROR("    %s", error.message.c_str());
			}
			strongThis->failedCallback_(
				SessionAuthResponse::BroadcastFailed);
		}
	};

	auto endedHandler = [weakThis, tryNum](
				    WebsocketClient::ConnectionEndType endType) {
		if (auto strongThis = weakThis.lock()) {
			switch (strongThis->broadcastState_) {
			case BroadcastState::Starting:
			case BroadcastState::Streaming:
			case BroadcastState::Live:
				if (endType ==
				    WebsocketClient::ConnectionEndType::Closed) {
					CAFF_LOG_INFO(
						"Stage websocket was closed.");
					// retry immediately
					strongThis->setupSubscription();
					return;
				}
				CAFF_LOG_ERROR("Stage websocket failed.");
				// retry after delay below
				break;
			case BroadcastState::Offline:
			case BroadcastState::Stopping:
				CAFF_LOG_INFO(
					"Broadcast is offline. Subscription ended.");
				return;
			}
		} else {
			CAFF_LOG_INFO(
				"Broadcast no longer exists. Subscription ended.");
			return;
		}

		auto sleepFor = backoffDuration(tryNum);
		CAFF_LOG_INFO(
			"Retrying broadcast subscription in %lld seconds.",
			sleepFor.count());
		std::this_thread::sleep_for(sleepFor);

		if (auto strongThis = weakThis.lock()) {
			switch (strongThis->broadcastState_) {
			case BroadcastState::Starting:
			case BroadcastState::Streaming:
			case BroadcastState::Live:
				CAFF_LOG_DEBUG(
					"Retrying broadcast subscription.");
				strongThis->setupSubscription(tryNum + 1);
				return;
			case BroadcastState::Offline:
			case BroadcastState::Stopping:
				CAFF_LOG_DEBUG(
					"Broadcast offline. Retry canceled.");
				return;
			}
		} else {
			CAFF_LOG_INFO(
				"Broadcast no longer exists. Retry canceled.");
		}
	};

	subscription_ = std::make_shared<
		GraphqlSubscription<caffql::Subscription::StageField>>(
		websocketClient_, sharedCredentials_, "stage " + username_,
		messageHandler, endedHandler, clientId_,
		caffql::ClientType::Obsplugin, clientType, username_,
		std::nullopt, std::nullopt, false, std::nullopt);
	subscription_->connect();
}

} // namespace caff
