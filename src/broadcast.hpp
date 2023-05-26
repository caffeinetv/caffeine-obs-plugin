#pragma once

#include <atomic>
#include <memory>
#include <optional>

#include <functional>
#include <future>
#include <vector>

#include "error-logging.hpp"
#include "websocket-api.hpp"

namespace caff {
class Broadcast : public std::enable_shared_from_this<Broadcast> {
public:
	Broadcast(SharedCredentials &sharedCredentials, std::string username,
		  std::string title,
		  std::optional<std::vector<std::string>> hashtags,
		  std::optional<std::string> category);

	~Broadcast() = default;

	void start(std::function<void()> startedCallback,
		   std::function<void(SessionAuthResponse)> failedCallback);
	void stop();
	std::variant<std::string, SessionAuthResponse> createSrtFeed();

	bool setTitle(std::string title);
	bool setContentRating(caffql::ContentRating contentRating);
	bool setThumbnailImage(std::string filename);
	ConnectionQuality getConnectionQuality();
	std::string const &getClientId() const { return clientId_; }

private:
	enum class BroadcastState {
		Offline,
		Starting,
		Streaming,
		Live,
		Stopping
	};
	std::atomic<BroadcastState> broadcastState_{BroadcastState::Offline};
	static char const *broadcastStateString(BroadcastState broadcastState);

	std::thread broadcastThread_;
	WebsocketClient websocketClient_;
	std::mutex mutex;

	std::function<void(SessionAuthResponse)> failedCallback_;

	SharedCredentials &sharedCredentials_;
	std::string clientId_;
	std::string username_;
	std::string title_;
	std::string thumbnailFileName_;
	caffql::ContentRating contentRating_;
	std::string feedId_;
	ConnectionQuality connectionQuality_ = ConnectionQualityUnknown;
	std::optional<std::string> broadcastId_;
	std::string streamUrl_;
	std::string heartbeatUrl_;
	std::string snapshotUrl_;
	std::optional<std::vector<std::string>> hashtags_;
	std::optional<std::string> category_;

	std::shared_ptr<GraphqlSubscription<caffql::Subscription::StageField>>
		subscription_;

	enum class SubscriptionState {
		None,
		Open,
		FeedHasAppeared,
		StageHasGoneLive
	};
	SubscriptionState subscriptionState_{};
	std::promise<bool> subscriptionOpened_;

	bool requireBroadcastState(BroadcastState expectedBroadcastState) const;
	bool transitionBroadcastState(BroadcastState expectedBroadcastState,
				      BroadcastState newBroadcastState);
	bool isOnline() const;

	void startHeartbeat(std::function<void()> startedCallback);
	caffql::FeedInput currentFeedInput();
	caffql::SocialMetadataInput currentSocialMetadataInput();
	std::string fullTitle();
	bool updateFeed();
	bool updateTitle();
	bool updateContentRating();
	void setupSubscription(size_t tryNum = 1);
	std::vector<uint8_t> createSnapshot();
};
}
