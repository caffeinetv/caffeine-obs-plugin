#pragma once

#include <string>
#include <vector>

namespace caff {
// Stage
char constexpr defaultTitle[] = "LIVE on Caffeine";
size_t constexpr maxTitleLength = 255;

// Helpers
std::string annotateTitle(std::string title);
bool isOlderVersion(std::string currentVersion, std::string latestVersion);
std::vector<std::string> splitHashtags(std::string hashtags);
} // namespace caff