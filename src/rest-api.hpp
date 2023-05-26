#pragma once

#include "caffeine.hpp"
#include "error-logging.hpp"
#include "real-time.hpp"

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "nlohmann/json.hpp"

namespace caff {
using Json = nlohmann::json;

extern std::string clientType;
extern std::string clientVersion;

struct Credentials {
	std::string accessToken;
	std::string refreshToken;
	std::string caid;
	std::string credential;
};

class SharedCredentials;

class LockedCredentials {
public:
	Credentials &credentials;

private:
	friend class SharedCredentials;
	explicit LockedCredentials(SharedCredentials &credentials);
	std::unique_lock<std::mutex> lock;
};

class SharedCredentials {
public:
	explicit SharedCredentials(Credentials credentials);
	LockedCredentials lock();

private:
	friend class LockedCredentials;
	Credentials credentials;
	std::mutex mutex;
};

struct AuthResponse {
	SessionAuthResponse result = Failure;
	std::optional<Credentials> credentials;
};

struct UserInfo {
	std::string username;
	bool canBroadcast;
};

struct HeartbeatResponse {
	std::string id;
	bool isReadyForViewing;
	std::optional<bool> srtConnected;
	std::string stageId;
};

struct CategoryResponse {
	std::string title;
	std::string tag;
};

SessionAuthResponse checkInternetConnection();
SessionAuthResponse checkCaffeineConnection();

AuthResponse signIn(char const *username, char const *password,
		    char const *otp);

AuthResponse refreshAuth(char const *refreshToken);

bool refreshCredentials(SharedCredentials &creds);

std::optional<UserInfo> getUserInfo(SharedCredentials &creds);

std::optional<Json> graphqlRawRequest(SharedCredentials &creds,
				      Json const &requestJson);

std::chrono::duration<long long> backoffDuration(size_t tryNum);
std::optional<HeartbeatResponse> heartbeatStream(std::string const &streamUrl,
						 SharedCredentials &creds);
bool updateSnapshot(std::string broadcastId,
		    std::vector<uint8_t> const &snapshotImageData,
		    SharedCredentials &sharedCreds);
bool updateSnapshotFile(std::string broadcastId,
			std::string const &snapshotFileName,
			SharedCredentials &sharedCreds);
std::vector<uint8_t> getPreviewImage(std::string const &streamUrl,
				     SharedCredentials &creds);
SessionAuthResponse checkMinimumSupportedVersion();
SessionAuthResponse checkLatestVersion();

std::vector<CategoryResponse> categoryList(SharedCredentials &creds);

template<typename OperationField, typename... Args>
std::optional<typename OperationField::ResponseData>
graphqlRequest(SharedCredentials &creds, Args const &...args)
{
	static_assert(
		OperationField::operation != caffql::Operation::Subscription,
		"graphqlRequest only supports query and mutation operations");

	auto requestJson = OperationField::request(args...);
	auto rawResponse = graphqlRawRequest(creds, requestJson);

	if (!rawResponse) {
		return {};
	}

	caffql::GraphqlResponse<typename OperationField::ResponseData> response;

	try {
		response = OperationField::response(rawResponse);
	} catch (...) {
		CAFF_LOG_ERROR("Failed to unpack graphql response");
		return {};
	}

	if (auto data = std::get_if<typename OperationField::ResponseData>(
		    &response)) {
		return std::move(*data);
	} else {
		auto const &errors =
			std::get<std::vector<caffql::GraphqlError>>(response);

		if (errors.empty()) {
			CAFF_LOG_ERROR("Empty error list for graphql request");
		}

		for (auto const &error : errors) {
			CAFF_LOG_ERROR(
				"Encountered graphql error with message: %s",
				error.message.c_str());
		}

		return {};
	}
}

} // namespace caff
