#pragma once

#include "rest-api.hpp"

#include "nlohmann/json.hpp"

// C datatype serialization has to be outside of caff namespace

NLOHMANN_JSON_SERIALIZE_ENUM(ConnectionQuality,
			     {{ConnectionQualityGood, "GOOD"},
			      {ConnectionQualityPoor, "POOR"},
			      {ConnectionQualityUnknown, nullptr}})

namespace caff {
using Json = nlohmann::json;

template<typename T>
inline void get_value_to(Json const &json, char const *key, T &target)
{
	json.at(key).get_to(target);
}

template<typename T>
inline void get_value_to(Json const &json, char const *key,
			 std::optional<T> &target)
{
	auto it = json.find(key);
	if (it != json.end()) {
		it->get_to(target);
	} else {
		target.reset();
	}
}

template<typename T>
inline void set_value_from(Json &json, char const *key, T const &source)
{
	json[key] = source;
}

template<typename T>
inline void set_value_from(Json &json, char const *key,
			   std::optional<T> const &source)
{
	if (source) {
		json[key] = *source;
		return;
	}
	auto it = json.find(key);
	if (it != json.end()) {
		json.erase(it);
	}
}

void from_json(Json const &json, Credentials &credentials);
void from_json(Json const &json, UserInfo &userInfo);
void from_json(Json const &json, HeartbeatResponse &response);
} // namespace caff
