#include "urls.hpp"
#include "utils.hpp"

#include <stdlib.h>

namespace caff {

const std::string caffeineDomain = []() -> std::string {
	auto custom = getenv("LIBCAFFEINE_DOMAIN");
	if (!isEmpty(custom)) {
		return custom;
	}
	return "caffeine.tv";
}();

std::string const apiEndpoint = "https://api." + caffeineDomain;
std::string const realtimeEndpoint = "https://realtime." + caffeineDomain;
std::string const eventsEndpoint = "https://events." + caffeineDomain;
std::string const lakituEndpoint = "https://lakitu." + caffeineDomain;

std::string const versionCheckUrl = apiEndpoint + "/v1/version-check";
std::string const signInUrl = apiEndpoint + "/v1/account/signin";
std::string const refreshTokenUrl = apiEndpoint + "/v1/account/token";

std::string const realtimeGraphqlUrl =
	realtimeEndpoint + "/public/graphql/query";

std::string const broadcastMetricsUrl =
	eventsEndpoint + "/v1/broadcast_metrics";

std::string const healthCheckUrl = apiEndpoint + "/healthcheck";

std::string const latestVersionCheckUrl =
	"https://api.github.com/repos/caffeinetv/caffeine-obs-plugin/releases/latest";

std::string const categoryListUrl = apiEndpoint + "/social/v1/categories";

std::string getUserUrl(std::string const &id)
{
	return apiEndpoint + "/v1/users/" + id;
}

std::string broadcastUrl(std::string const &id)
{
	return apiEndpoint + "/v1/broadcasts/" + id;
}

} // namespace caff