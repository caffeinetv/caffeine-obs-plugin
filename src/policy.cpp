#include "policy.hpp"
#include "utils.hpp"
#include <algorithm>
#include <sstream>

namespace caff {
std::string annotateTitle(std::string title)
{
	trim(title);
	if (title.empty()) {
		title = defaultTitle;
	}

	if (title.length() > maxTitleLength) {
		title.resize(maxTitleLength);
	}

	return title;
}

// Version is in format 0.0.0
bool isOlderVersion(std::string currentVersion, std::string latestVersion)
{
	currentVersion.erase(std::remove(currentVersion.begin(),
					 currentVersion.end(), '.'),
			     currentVersion.end());
	int currentNum = std::stoi(currentVersion);

	latestVersion.erase(std::remove(latestVersion.begin(),
					latestVersion.end(), '.'),
			    latestVersion.end());
	int latestNum = std::stoi(latestVersion);

	if (currentNum < latestNum) {
		return true;
	}
	return false;
}

std::vector<std::string> splitHashtags(std::string hashtags)
{
	// Replace whitespaces with #
	std::replace(hashtags.begin(), hashtags.end(), ' ', '#');
	std::vector<std::string> tokens;
	std::string token;

	std::istringstream tokenStream(hashtags);

	while (std::getline(tokenStream, token, '#')) {
		if (token != "") {
			// Trim white spaces
			token.erase(std::remove_if(token.begin(), token.end(),
						   [](unsigned char x) {
							   return std::isspace(
								   x);
						   }),
				    token.end());
			tokens.push_back(token);
		}
	}
	return tokens;
}

} // namespace caff