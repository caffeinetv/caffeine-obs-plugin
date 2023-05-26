#include "serialization.hpp"

#include <sstream>

namespace caff {
void from_json(Json const &json, Credentials &credentials)
{
	get_value_to(json, "access_token", credentials.accessToken);
	get_value_to(json, "refresh_token", credentials.refreshToken);
	get_value_to(json, "caid", credentials.caid);
	get_value_to(json, "credential", credentials.credential);
}

void from_json(Json const &json, UserInfo &userInfo)
{
	caff::get_value_to(json, "username", userInfo.username);
	caff::get_value_to(json, "can_broadcast", userInfo.canBroadcast);
}

void from_json(Json const &json, HeartbeatResponse &response)
{
	get_value_to(json, "id", response.id);
	get_value_to(json, "is_ready_for_viewing", response.isReadyForViewing);
	get_value_to(json, "srt_connected", response.srtConnected);
	get_value_to(json, "stage_id", response.stageId);
}

} // namespace caff