#pragma once

#ifndef INSTANCE_H
#define INSTANCE_H

#include <functional>
#include <optional>
#include <string>
#include <thread>
#include <mutex>

#include "rest-api.hpp"
#include "caffeine.hpp"

namespace caff {
class Broadcast;

class Instance {
public:
	Instance(Instance &other) = delete;
	void operator=(const Instance &) = delete;

	static Instance *getInstance();

	SessionAuthResponse signIn(char const *username, char const *password,
				   char const *otp);
	SessionAuthResponse refreshAuth(char const *refreshToken);
	bool isSignedIn() const;
	void signOut();

	char const *getUsername() const;
	char const *getRefreshToken() const;
	char const *getAccessToken() const;
	char const *getCredential() const;

	// Streaming
	bool canBroadcast() const;

	SessionAuthResponse
	startBroadcast(std::function<void()> startedCallback,
		       std::function<void(SessionAuthResponse)> failedCallback);

	std::variant<std::string, SessionAuthResponse>
	createSrtFeed(std::string title, std::string username,
		      std::optional<std::vector<std::string>> hashtags,
		      std::optional<std::string> category);

	std::shared_ptr<Broadcast> getBroadcast();

	void endBroadcast();
	SessionAuthResponse checkRecentVersion();
	SessionAuthResponse checkMinSupportedVersion();
	bool setBroadcastTitle(std::string title);
	bool setRating(caffql::ContentRating rating);
	bool setThumbnail(std::string filename);
	std::vector<CategoryResponse> categories();

private:
	static Instance *instance_;
	static std::mutex mutex_;
	SessionAuthResponse
	authenticate(std::function<AuthResponse()> signinFunc);

	// Member variables
	mutable std::optional<SharedCredentials> sharedCredentials_;
	std::optional<std::string> refreshToken_;
	std::optional<UserInfo> userInfo_;

	// Streaming
	std::shared_ptr<Broadcast> broadcast_;
	std::mutex broadcastMutex_;

protected:
	Instance() = default;
	~Instance() = default;
};

} // namespace caff

#endif // INSTANCE_H