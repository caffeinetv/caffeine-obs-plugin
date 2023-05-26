#pragma once

#include <string>

namespace caff {
// Set the environment variable LIBCAFFEINE_DOMAIN to use a custom environment.
// The default environment is "caffeine.tv".

extern std::string const caffeineDomain;
extern std::string const versionCheckUrl;
extern std::string const signInUrl;
extern std::string const refreshTokenUrl;
extern std::string const realtimeGraphqlUrl;
extern std::string const broadcastMetricsUrl;
extern std::string const healthCheckUrl;
extern std::string const latestVersionCheckUrl;
extern std::string const categoryListUrl;

std::string getUserUrl(std::string const &id);
std::string broadcastUrl(std::string const &id);
} // namespace caff
