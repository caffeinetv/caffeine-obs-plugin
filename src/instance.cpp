#include "instance.hpp"
#include "broadcast.hpp"

#include <iostream>

namespace caff {
Instance *Instance::instance_ = nullptr;
std::mutex Instance::mutex_;

Instance *Instance::getInstance()
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (instance_ == nullptr) {
		instance_ = new Instance;
	}
	return instance_;
}

SessionAuthResponse Instance::signIn(char const *username, char const *password,
				     char const *otp)
{
	return authenticate(
		[=] { return caff::signIn(username, password, otp); });
}

SessionAuthResponse Instance::refreshAuth(char const *refreshToken)
{
	return authenticate([=] { return caff::refreshAuth(refreshToken); });
}

bool Instance::isSignedIn() const
{
	return sharedCredentials_.has_value() && userInfo_.has_value();
}

void Instance::signOut()
{
	sharedCredentials_.reset();
	refreshToken_.reset();
	userInfo_.reset();
}

char const *Instance::getRefreshToken() const
{
	return refreshToken_ ? refreshToken_->c_str() : nullptr;
}

char const *Instance::getAccessToken() const
{
	if (!sharedCredentials_) {
		return nullptr;
	}
	static std::string copyForC;
	copyForC = sharedCredentials_->lock().credentials.accessToken;
	return copyForC.c_str();
}

char const *Instance::getCredential() const
{
	if (!sharedCredentials_) {
		return nullptr;
	}
	static std::string copyForC;
	copyForC = sharedCredentials_->lock().credentials.credential;
	return copyForC.c_str();
}

char const *Instance::getUsername() const
{
	return userInfo_ ? userInfo_->username.c_str() : nullptr;
}

bool Instance::canBroadcast() const
{
	return userInfo_ ? userInfo_->canBroadcast : false;
}

SessionAuthResponse Instance::startBroadcast(
	std::function<void()> startedCallback,
	std::function<void(SessionAuthResponse)> failedCallback)
{
	if (!isSignedIn()) {
		return SessionAuthResponse::NotSignedIn;
	}
	if (!userInfo_->canBroadcast) {
		// Refresh user info in case the user is newly allowed to broadcast
		auto newUserInfo = caff::getUserInfo(*sharedCredentials_);
		if (!newUserInfo || !newUserInfo->canBroadcast) {
			return SessionAuthResponse::EmailVerificationRequired;
		}
		userInfo_ = std::move(newUserInfo);
	}

	std::lock_guard<std::mutex> lock(broadcastMutex_);

	broadcast_->start(startedCallback, failedCallback);
	return SessionAuthResponse::Success;
}

std::variant<std::string, SessionAuthResponse>
Instance::createSrtFeed(std::string title, std::string username,
			std::optional<std::vector<std::string>> hashtags,
			std::optional<std::string> category)
{
	if (!isSignedIn()) {
		return SessionAuthResponse::NotSignedIn;
	}
	CAFF_LOG_DEBUG("Caffeine started to create feed v2");

	std::lock_guard<std::mutex> lock(broadcastMutex_);
	broadcast_ = std::make_shared<Broadcast>(*sharedCredentials_, username,
						 std::move(title), hashtags,
						 category);

	auto result = broadcast_->createSrtFeed();
	return result;
}

std::shared_ptr<Broadcast> Instance::getBroadcast()
{
	std::lock_guard<std::mutex> lock(broadcastMutex_);
	return broadcast_;
}

void Instance::endBroadcast()
{
	std::lock_guard<std::mutex> lock(broadcastMutex_);
	if (broadcast_) {
		broadcast_->stop();
		// Asynchronously destroy the broadcast so we don't block this thread
		std::thread([broadcast_ = std::move(broadcast_)] {}).detach();
	}
}

SessionAuthResponse Instance::checkRecentVersion()
{
	return checkLatestVersion();
}

SessionAuthResponse Instance::checkMinSupportedVersion()
{
	return checkMinimumSupportedVersion();
}

bool Instance::setBroadcastTitle(std::string title)
{
	if (broadcast_) {
		return broadcast_->setTitle(title);
	}
	// No updates needed when not broadcasting
	return true;
}

bool Instance::setRating(caffql::ContentRating rating)
{
	if (broadcast_) {
		return broadcast_->setContentRating(rating);
	}
	// No updates needed when not broadcasting
	return true;
}

bool Instance::setThumbnail(std::string filename)
{
	if (broadcast_ && !filename.empty()) {
		return broadcast_->setThumbnailImage(filename);
	}
	// No updates needed when not broadcasting
	return true;
}

std::vector<CategoryResponse> Instance::categories()
{
	if (!sharedCredentials_) {
		return {};
	}
	return caff::categoryList(*sharedCredentials_);
}

SessionAuthResponse
Instance::authenticate(std::function<AuthResponse()> authFunc)
{
	if (broadcast_) {
		CAFF_LOG_ERROR(
			"Attempting to authenticate while broadcast is already active");
		return SessionAuthResponse::AlreadyBroadcasting;
	}
	auto response = authFunc();
	if (!response.credentials) {
		return response.result;
	}
	refreshToken_ = response.credentials->refreshToken;
	sharedCredentials_.emplace(std::move(*response.credentials));
	userInfo_ = caff::getUserInfo(*sharedCredentials_);
	if (!userInfo_) {
		sharedCredentials_.reset();
		this->refreshToken_.reset();
		return Failure;
	}
	return response.result;
}

} // namespace caff