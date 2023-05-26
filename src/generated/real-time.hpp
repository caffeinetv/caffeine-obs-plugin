// This file was automatically generated and should not be edited.
#pragma once

#include <memory>
#include <vector>
#include "nlohmann/json.hpp"
#include <optional>
#include <variant>

// optional serialization
namespace nlohmann {
template<typename T> struct adl_serializer<std::optional<T>> {
	static void to_json(json &json, std::optional<T> const &opt)
	{
		if (opt.has_value()) {
			json = *opt;
		} else {
			json = nullptr;
		}
	}

	static void from_json(const json &json, std::optional<T> &opt)
	{
		if (json.is_null()) {
			opt.reset();
		} else {
			opt = json.get<T>();
		}
	}
};
}

namespace caffql {

using Json = nlohmann::json;
using Id = std::string;
using std::optional;
using std::variant;
using std::monostate;
using std::visit;

enum class Operation { Query, Mutation, Subscription };

struct GraphqlError {
	std::string message;
};

template<typename Data>
using GraphqlResponse = variant<Data, std::vector<GraphqlError>>;

inline void from_json(Json const &json, GraphqlError &value)
{
	json.at("message").get_to(value.message);
}

// For sending data into the CDN.
struct BroadcasterStream {
	Id id;
	std::string url;
	std::string sdpAnswer;
};

inline void from_json(Json const &json, BroadcasterStream &value)
{
	json.at("id").get_to(value.id);
	json.at("url").get_to(value.url);
	json.at("sdpAnswer").get_to(value.sdpAnswer);
}

enum class Capability { Audio, Video, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(Capability, {
						 {Capability::Unknown, nullptr},
						 {Capability::Audio, "AUDIO"},
						 {Capability::Video, "VIDEO"},
					 });

/*
    This error will be returned when Reyes failed to retrieve streams for all of
    the feeds even after retrying.
    */
struct CdnError {
	optional<std::string> title;
	std::string message;
};

inline void from_json(Json const &json, CdnError &value)
{
	{
		auto it = json.find("title");
		if (it != json.end()) {
			it->get_to(value.title);
		} else {
			value.title.reset();
		}
	}
	json.at("message").get_to(value.message);
}

/*
    Reyes will now enforce rules around when and how clients are able to take over
    the stage. (See documentation
    https://github.com/caffeinetv/reyes/blob/master/docs/client-contention.md).
    
    For example, if you try to start a broadcaston Lucio and Reyes does not allow
    it, the user will receive a message via this Error type.
    */
struct ClientContentionError {
	optional<std::string> title;
	std::string message;
};

inline void from_json(Json const &json, ClientContentionError &value)
{
	{
		auto it = json.find("title");
		if (it != json.end()) {
			it->get_to(value.title);
		} else {
			value.title.reset();
		}
	}
	json.at("message").get_to(value.message);
}

enum class ClientType {
	Web,
	// iOS and Android
	Mobile,
	// COBS and Lucio (via libcaffeine)
	Capture,
	Obsplugin,
	Unknown = -1
};

NLOHMANN_JSON_SERIALIZE_ENUM(ClientType,
			     {
				     {ClientType::Unknown, nullptr},
				     {ClientType::Web, "WEB"},
				     {ClientType::Mobile, "MOBILE"},
				     {ClientType::Capture, "CAPTURE"},
				     {ClientType::Obsplugin, "OBSPLUGIN"},
			     });

// ContentRating communicates if the content isn't appropriate for certain groups.
enum class ContentRating { Everyone, SeventeenPlus, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(ContentRating,
			     {
				     {ContentRating::Unknown, nullptr},
				     {ContentRating::Everyone, "EVERYONE"},
				     {ContentRating::SeventeenPlus,
				      "SEVENTEEN_PLUS"},
			     });

struct ControllingClient {
	Id clientId;
	ClientType clientType;
};

inline void from_json(Json const &json, ControllingClient &value)
{
	json.at("clientId").get_to(value.clientId);
	json.at("clientType").get_to(value.clientType);
}

/*
    GameInput is used in order to allow both setting and clearing the game ID.
    
    To set the game ID, include a GameInput input with a non-null ID.
    To clear the game ID, include a GameInput input with a null ID.
    To leave it unchanged, then don't include the GameInput.
    */
struct GameInput {
	optional<Id> id;
};

inline void to_json(Json &json, GameInput const &value)
{
	json["id"] = value.id;
}

/*
    This error will be returned when the user is not authorized to view requested
    content due to the user's location.
    */
struct GeoRestrictionError {
	optional<std::string> title;
	std::string message;
};

inline void from_json(Json const &json, GeoRestrictionError &value)
{
	{
		auto it = json.find("title");
		if (it != json.end()) {
			it->get_to(value.title);
		} else {
			value.title.reset();
		}
	}
	json.at("message").get_to(value.message);
}

struct LiveHostable {
	Id address;
};

inline void from_json(Json const &json, LiveHostable &value)
{
	json.at("address").get_to(value.address);
}

struct LiveHosting {
	Id address;
	double volume;
	/*
        ownerId is the account ID of the owner of the stage that is providing the
        underlying live-hostable content.
        */
	std::string ownerId;
	/*
        ownerUsername is the username of the owner of the stage that is providing the
        underlying live-hostable content.
        */
	std::string ownerUsername;
};

inline void from_json(Json const &json, LiveHosting &value)
{
	json.at("address").get_to(value.address);
	json.at("volume").get_to(value.volume);
	json.at("ownerId").get_to(value.ownerId);
	json.at("ownerUsername").get_to(value.ownerUsername);
}

/*
    This is an error specifically for when we have no more capacity. Clients should
    not retry when they get this.
    */
struct OutOfCapacityError {
	optional<std::string> title;
	std::string message;
};

inline void from_json(Json const &json, OutOfCapacityError &value)
{
	{
		auto it = json.find("title");
		if (it != json.end()) {
			it->get_to(value.title);
		} else {
			value.title.reset();
		}
	}
	json.at("message").get_to(value.message);
}

// For pay per view model.
struct Paywall {
	/*
        The id that represents the ticket/prop that would need to be purchased in order
        to consume the content.
        */
	std::string ppvShowId;
	// Specific title to display for the paywall.
	std::string title;
	// Price of the ticket, in caffeine gold, for display purposes.
	optional<int32_t> price;
	/*
        Indicates whether the current paywall has been enforced. 
        
        If true, means that only users with a valid ticket will be able to access the content from here on now. 
        This field can be toggled true/false during the show.
        */
	bool enforced;
	// Specific header to display for the paywall.
	std::string header;
	// Specific description to display for the paywall.
	std::string description;
	// Specific image to display on the paywall.
	std::string previewImagePath;
};

inline void from_json(Json const &json, Paywall &value)
{
	json.at("ppvShowId").get_to(value.ppvShowId);
	json.at("title").get_to(value.title);
	{
		auto it = json.find("price");
		if (it != json.end()) {
			it->get_to(value.price);
		} else {
			value.price.reset();
		}
	}
	json.at("enforced").get_to(value.enforced);
	json.at("header").get_to(value.header);
	json.at("description").get_to(value.description);
	json.at("previewImagePath").get_to(value.previewImagePath);
}

struct PaywallInput {
	std::string username;
	std::string title;
	std::string ppvShowId;
	optional<std::string> premiumVideoUrl;
	optional<std::string> header;
	optional<std::string> description;
	optional<int32_t> price;
	optional<std::string> previewImagePath;
};

inline void to_json(Json &json, PaywallInput const &value)
{
	json["username"] = value.username;
	json["title"] = value.title;
	json["ppvShowId"] = value.ppvShowId;
	json["premiumVideoUrl"] = value.premiumVideoUrl;
	json["header"] = value.header;
	json["description"] = value.description;
	json["price"] = value.price;
	json["previewImagePath"] = value.previewImagePath;
}

struct RefSocialActivity {
	Id broadcastId;
	Id activityId;
};

inline void to_json(Json &json, RefSocialActivity const &value)
{
	json["broadcastId"] = value.broadcastId;
	json["activityId"] = value.activityId;
}

struct ReplayData {
	std::string adminReplayHLSURL;
	std::string originalTitle;
	std::string originalDate;
	Id originalBroadcastId;
	std::string scheduledStartTime;
	optional<std::string> scheduledEndTime;
};

inline void from_json(Json const &json, ReplayData &value)
{
	json.at("adminReplayHLSURL").get_to(value.adminReplayHLSURL);
	json.at("originalTitle").get_to(value.originalTitle);
	json.at("originalDate").get_to(value.originalDate);
	json.at("originalBroadcastId").get_to(value.originalBroadcastId);
	json.at("scheduledStartTime").get_to(value.scheduledStartTime);
	{
		auto it = json.find("scheduledEndTime");
		if (it != json.end()) {
			it->get_to(value.scheduledEndTime);
		} else {
			value.scheduledEndTime.reset();
		}
	}
}

struct ReplayInput {
	std::string replayUrl;
	std::string originalTitle;
	std::string originalBroadcastDate;
	Id originalBroadcastId;
	std::string scheduledStartTime;
	optional<std::string> scheduledEndTime;
};

inline void to_json(Json &json, ReplayInput const &value)
{
	json["replayUrl"] = value.replayUrl;
	json["originalTitle"] = value.originalTitle;
	json["originalBroadcastDate"] = value.originalBroadcastDate;
	json["originalBroadcastId"] = value.originalBroadcastId;
	json["scheduledStartTime"] = value.scheduledStartTime;
	json["scheduledEndTime"] = value.scheduledEndTime;
}

struct RequestViewerStreamPayload {
	optional<Id> feedId;
};

inline void from_json(Json const &json, RequestViewerStreamPayload &value)
{
	{
		auto it = json.find("feedId");
		if (it != json.end()) {
			it->get_to(value.feedId);
		} else {
			value.feedId.reset();
		}
	}
}

enum class Restriction { UsOnly, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(Restriction,
			     {
				     {Restriction::Unknown, nullptr},
				     {Restriction::UsOnly, "US_ONLY"},
			     });

enum class Role { Primary, Secondary, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(Role, {
					   {Role::Unknown, nullptr},
					   {Role::Primary, "PRIMARY"},
					   {Role::Secondary, "SECONDARY"},
				   });

struct S3Fields {
	std::string key;
	std::string contentType;
	std::string policy;
	std::string xAmzCredential;
	std::string xAmzAlgorithm;
	std::string xAmzDate;
	std::string xAmzSignature;
	std::string xAmzSessionToken;
};

inline void from_json(Json const &json, S3Fields &value)
{
	json.at("key").get_to(value.key);
	json.at("contentType").get_to(value.contentType);
	json.at("policy").get_to(value.policy);
	json.at("xAmzCredential").get_to(value.xAmzCredential);
	json.at("xAmzAlgorithm").get_to(value.xAmzAlgorithm);
	json.at("xAmzDate").get_to(value.xAmzDate);
	json.at("xAmzSignature").get_to(value.xAmzSignature);
	json.at("xAmzSessionToken").get_to(value.xAmzSessionToken);
}

struct S3UploadFieldsPayload {
	std::string username;
	std::string url;
	S3Fields fields;
};

inline void from_json(Json const &json, S3UploadFieldsPayload &value)
{
	json.at("username").get_to(value.username);
	json.at("url").get_to(value.url);
	json.at("fields").get_to(value.fields);
}

struct SetReplayUrlAndReplaySchedulePayload {
	std::string originalTitle;
	std::string originalBroadcastDate;
	Id originalBroadcastId;
	std::string scheduledStartTime;
	std::string scheduledEndTime;
};

inline void from_json(Json const &json,
		      SetReplayUrlAndReplaySchedulePayload &value)
{
	json.at("originalTitle").get_to(value.originalTitle);
	json.at("originalBroadcastDate").get_to(value.originalBroadcastDate);
	json.at("originalBroadcastId").get_to(value.originalBroadcastId);
	json.at("scheduledStartTime").get_to(value.scheduledStartTime);
	json.at("scheduledEndTime").get_to(value.scheduledEndTime);
}

enum class ShareType {
	// Indicates to generate deep links for upcoming card share.
	Upcoming,
	Unknown = -1
};

NLOHMANN_JSON_SERIALIZE_ENUM(ShareType,
			     {
				     {ShareType::Unknown, nullptr},
				     {ShareType::Upcoming, "UPCOMING"},
			     });

struct SocialMetadataInput {
	RefSocialActivity inReactionTo;
	optional<std::vector<std::string>> hashtags;
	optional<std::string> category;
};

inline void to_json(Json &json, SocialMetadataInput const &value)
{
	json["inReactionTo"] = value.inReactionTo;
	json["hashtags"] = value.hashtags;
	json["category"] = value.category;
}

struct SocialShareFieldsInput {
	optional<std::string> ogTitle;
	optional<std::string> ogDescription;
	optional<std::string> ogImageUrl;
	optional<std::string> message;
};

inline void to_json(Json &json, SocialShareFieldsInput const &value)
{
	json["ogTitle"] = value.ogTitle;
	json["ogDescription"] = value.ogDescription;
	json["ogImageUrl"] = value.ogImageUrl;
	json["message"] = value.message;
}

enum class SourceConnectionQuality { Good, Poor, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(SourceConnectionQuality,
			     {
				     {SourceConnectionQuality::Unknown,
				      nullptr},
				     {SourceConnectionQuality::Good, "GOOD"},
				     {SourceConnectionQuality::Poor, "POOR"},
			     });

enum class SrtConnectionType { Active, Standby, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(SrtConnectionType,
			     {
				     {SrtConnectionType::Unknown, nullptr},
				     {SrtConnectionType::Active, "ACTIVE"},
				     {SrtConnectionType::Standby, "STANDBY"},
			     });

/*
    Indicates which badge asset should be shown in the UI. Clients should map this
    value to the appropriate asset.
    */
enum class StageBadge { Offline, Live, Replay, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(StageBadge,
			     {
				     {StageBadge::Unknown, nullptr},
				     {StageBadge::Offline, "OFFLINE"},
				     {StageBadge::Live, "LIVE"},
				     {StageBadge::Replay, "REPLAY"},
			     });

enum class StageShareChannel {
	Twitter,
	Facebook,
	Email,
	CopyLink,
	TextMessage,
	Default,
	Unknown = -1
};

NLOHMANN_JSON_SERIALIZE_ENUM(StageShareChannel,
			     {
				     {StageShareChannel::Unknown, nullptr},
				     {StageShareChannel::Twitter, "TWITTER"},
				     {StageShareChannel::Facebook, "FACEBOOK"},
				     {StageShareChannel::Email, "EMAIL"},
				     {StageShareChannel::CopyLink, "COPY_LINK"},
				     {StageShareChannel::TextMessage,
				      "TEXT_MESSAGE"},
				     {StageShareChannel::Default, "DEFAULT"},
			     });

struct StageShareLink {
	StageShareChannel channel;
	std::string deepLink;
	optional<std::string> inviteeId;
};

inline void from_json(Json const &json, StageShareLink &value)
{
	json.at("channel").get_to(value.channel);
	json.at("deepLink").get_to(value.deepLink);
	{
		auto it = json.find("inviteeId");
		if (it != json.end()) {
			it->get_to(value.inviteeId);
		} else {
			value.inviteeId.reset();
		}
	}
}

struct StageShareLinksPayload {
	std::vector<StageShareLink> stageShareLinks;
	std::string message;
};

inline void from_json(Json const &json, StageShareLinksPayload &value)
{
	json.at("stageShareLinks").get_to(value.stageShareLinks);
	json.at("message").get_to(value.message);
}

/*
    StageSource indicates how the client is connecting to the stage. It is used
    to distinguish between live in the lobby viewing and regular viewing on the
    stage.
    */
enum class StageSource { Stage, Lobby, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(StageSource,
			     {
				     {StageSource::Unknown, nullptr},
				     {StageSource::Stage, "STAGE"},
				     {StageSource::Lobby, "LOBBY"},
			     });

/*
    Represents the state of the stage. Which users are permitted to view the stage
    will depend on its state. The `live` boolean on the Stage type will now be
    derived from this state.
    */
enum class StageState {
	Offline,
	Test,
	BeforeShow,
	StartingSoon,
	Live,
	Replay,
	Unknown = -1
};

NLOHMANN_JSON_SERIALIZE_ENUM(StageState,
			     {
				     {StageState::Unknown, nullptr},
				     {StageState::Offline, "OFFLINE"},
				     {StageState::Test, "TEST"},
				     {StageState::BeforeShow, "BEFORE_SHOW"},
				     {StageState::StartingSoon,
				      "STARTING_SOON"},
				     {StageState::Live, "LIVE"},
				     {StageState::Replay, "REPLAY"},
			     });

struct StageStatePayload {
	optional<StageState> raw;
	StageState computed;
};

inline void from_json(Json const &json, StageStatePayload &value)
{
	{
		auto it = json.find("raw");
		if (it != json.end()) {
			it->get_to(value.raw);
		} else {
			value.raw.reset();
		}
	}
	json.at("computed").get_to(value.computed);
}

struct StageTrailerUploadPayload {
	std::string username;
	std::string url;
	S3Fields fields;
};

inline void from_json(Json const &json, StageTrailerUploadPayload &value)
{
	json.at("username").get_to(value.username);
	json.at("url").get_to(value.url);
	json.at("fields").get_to(value.fields);
}

struct TakedownAllReplaysAsAdminPayload {
	optional<bool> success;
};

inline void from_json(Json const &json, TakedownAllReplaysAsAdminPayload &value)
{
	{
		auto it = json.find("success");
		if (it != json.end()) {
			it->get_to(value.success);
		} else {
			value.success.reset();
		}
	}
}

struct TakedownStagePayload {
	optional<bool> success;
};

inline void from_json(Json const &json, TakedownStagePayload &value)
{
	{
		auto it = json.find("success");
		if (it != json.end()) {
			it->get_to(value.success);
		} else {
			value.success.reset();
		}
	}
}

// For viewing data from the CDN.
struct ViewerStream {
	Id id;
	std::string url;
	std::string sdpOffer;
};

inline void from_json(Json const &json, ViewerStream &value)
{
	json.at("id").get_to(value.id);
	json.at("url").get_to(value.url);
	json.at("sdpOffer").get_to(value.sdpOffer);
}

struct ViewerStreamInput {
	Id id;
	std::string url;
	std::string sdpOffer;
};

inline void to_json(Json &json, ViewerStreamInput const &value)
{
	json["id"] = value.id;
	json["url"] = value.url;
	json["sdpOffer"] = value.sdpOffer;
}

enum class ViewingOption { Webrtc, Hls, Unknown = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(ViewingOption,
			     {
				     {ViewingOption::Unknown, nullptr},
				     {ViewingOption::Webrtc, "WEBRTC"},
				     {ViewingOption::Hls, "HLS"},
			     });

enum class ViewingOptionRule {
	ForceNewViewersToHlsOnly,
	ForceNewViewersToHlsWithWebrtcOption,
	Unknown = -1
};

NLOHMANN_JSON_SERIALIZE_ENUM(
	ViewingOptionRule,
	{
		{ViewingOptionRule::Unknown, nullptr},
		{ViewingOptionRule::ForceNewViewersToHlsOnly,
		 "FORCE_NEW_VIEWERS_TO_HLS_ONLY"},
		{ViewingOptionRule::ForceNewViewersToHlsWithWebrtcOption,
		 "FORCE_NEW_VIEWERS_TO_HLS_WITH_WEBRTC_OPTION"},
	});

struct UnknownError {
	optional<std::string> title;
	std::string message;
};

struct Error {
	variant<OutOfCapacityError, CdnError, ClientContentionError,
		GeoRestrictionError, UnknownError>
		implementation;

	optional<std::string> const &title() const
	{
		return visit(
			[](auto const &implementation)
				-> optional<std::string> const & {
				return implementation.title;
			},
			implementation);
	}

	std::string const &message() const
	{
		return visit(
			[](auto const &implementation) -> std::string const & {
				return implementation.message;
			},
			implementation);
	}
};

inline void from_json(Json const &json, UnknownError &value)
{
	{
		auto it = json.find("title");
		if (it != json.end()) {
			it->get_to(value.title);
		} else {
			value.title.reset();
		}
	}
	json.at("message").get_to(value.message);
}

inline void from_json(Json const &json, Error &value)
{
	std::string occupiedType = json.at("__typename");
	if (occupiedType == "OutOfCapacityError") {
		value = {OutOfCapacityError(json)};
	} else if (occupiedType == "CdnError") {
		value = {CdnError(json)};
	} else if (occupiedType == "ClientContentionError") {
		value = {ClientContentionError(json)};
	} else if (occupiedType == "GeoRestrictionError") {
		value = {GeoRestrictionError(json)};
	} else {
		value = {UnknownError(json)};
	}
}

// Optional fields that are not set will be set to the specified default values.
struct FeedInput {
	// Must be a valid, lower-cased v4 UUID.
	Id id;
	optional<GameInput> game;
	// Defaults to PRIMARY.
	optional<Role> role;
	/*
        Currently, the volume only matters for live hosting content, so that clients
        can determine the right balance between the volume of live hosting content
        versus the client's content.
        
        Defaults to 1.0; currently only returned on a LiveHostingFeed.
        */
	optional<double> volume;
	// Defaults to GOOD.
	optional<SourceConnectionQuality> sourceConnectionQuality;
	// Defaults to [AUDIO, VIDEO].
	optional<std::vector<Capability>> capabilities;
	/*
        Currently, only Garrus may add live-hostable content, but this is included
        for testing. Users without the permission will not be able to make the feed
        hostable.
        */
	optional<bool> liveHostable;
	/*
        Use this for adding a live hosting feed to the stage.
        
        Must correspond to a valid "address" on a LiveHostableFeed.
        */
	optional<Id> liveHostingAddress;
	/*
        Include the SDP offer when adding a new feed whose content you own and will
        be sending into the CDN. The SDP offer is used to allocate a
        broadcaster stream for the feed.
        
        NOTE: You must either provide an sdpOffer or a viewerStream.
        */
	optional<std::string> sdpOffer;
	/*
        If you want to add a live-hosting feed, and you already have a viewer stream
        that you want to reuse, include it here.
        */
	optional<ViewerStreamInput> viewerStream;
	/*
        If this feed will use SRT for ingest, then sdpOffer and viewerStream
        must be un-set, and srtConnectionType needs to be set to a list of the
        stream types being requested.
        */
	optional<std::vector<SrtConnectionType>> srtConnectionTypes;
};

inline void to_json(Json &json, FeedInput const &value)
{
	json["id"] = value.id;
	json["game"] = value.game;
	json["role"] = value.role;
	json["volume"] = value.volume;
	json["sourceConnectionQuality"] = value.sourceConnectionQuality;
	json["capabilities"] = value.capabilities;
	json["liveHostable"] = value.liveHostable;
	json["liveHostingAddress"] = value.liveHostingAddress;
	json["sdpOffer"] = value.sdpOffer;
	json["viewerStream"] = value.viewerStream;
	json["srtConnectionTypes"] = value.srtConnectionTypes;
}

struct InviteLinksInput {
	Id inviteeId;
	std::vector<StageShareChannel> channels;
};

inline void to_json(Json &json, InviteLinksInput const &value)
{
	json["inviteeId"] = value.inviteeId;
	json["channels"] = value.channels;
}

struct UnknownLiveHost {
	Id address;
};

struct LiveHost {
	variant<LiveHostable, LiveHosting, UnknownLiveHost> implementation;

	Id const &address() const
	{
		return visit(
			[](auto const &implementation) -> Id const & {
				return implementation.address;
			},
			implementation);
	}
};

inline void from_json(Json const &json, UnknownLiveHost &value)
{
	json.at("address").get_to(value.address);
}

inline void from_json(Json const &json, LiveHost &value)
{
	std::string occupiedType = json.at("__typename");
	if (occupiedType == "LiveHostable") {
		value = {LiveHostable(json)};
	} else if (occupiedType == "LiveHosting") {
		value = {LiveHosting(json)};
	} else {
		value = {UnknownLiveHost(json)};
	}
}

struct PaywallPayload {
	bool success;
	std::string username;
	optional<std::string> premiumVideoUrl;
	optional<Paywall> paywall;
	optional<Error> error;
};

inline void from_json(Json const &json, PaywallPayload &value)
{
	json.at("success").get_to(value.success);
	json.at("username").get_to(value.username);
	{
		auto it = json.find("premiumVideoUrl");
		if (it != json.end()) {
			it->get_to(value.premiumVideoUrl);
		} else {
			value.premiumVideoUrl.reset();
		}
	}
	{
		auto it = json.find("paywall");
		if (it != json.end()) {
			it->get_to(value.paywall);
		} else {
			value.paywall.reset();
		}
	}
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
}

struct SelectViewingOptionPayload {
	optional<ViewingOption> viewingOption;
};

inline void from_json(Json const &json, SelectViewingOptionPayload &value)
{
	{
		auto it = json.find("viewingOption");
		if (it != json.end()) {
			it->get_to(value.viewingOption);
		} else {
			value.viewingOption.reset();
		}
	}
}

struct SocialShareFieldsAsAdminPayload {
	std::string username;
	optional<std::string> ogTitle;
	optional<std::string> ogDescription;
	optional<std::string> ogImageUrl;
	optional<std::string> message;
	optional<Error> error;
};

inline void from_json(Json const &json, SocialShareFieldsAsAdminPayload &value)
{
	json.at("username").get_to(value.username);
	{
		auto it = json.find("ogTitle");
		if (it != json.end()) {
			it->get_to(value.ogTitle);
		} else {
			value.ogTitle.reset();
		}
	}
	{
		auto it = json.find("ogDescription");
		if (it != json.end()) {
			it->get_to(value.ogDescription);
		} else {
			value.ogDescription.reset();
		}
	}
	{
		auto it = json.find("ogImageUrl");
		if (it != json.end()) {
			it->get_to(value.ogImageUrl);
		} else {
			value.ogImageUrl.reset();
		}
	}
	{
		auto it = json.find("message");
		if (it != json.end()) {
			it->get_to(value.message);
		} else {
			value.message.reset();
		}
	}
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
}

struct SrtConnection {
	SrtConnectionType type;
	std::string srtUrl;
	std::string heartbeatUrl;
	std::string snapshotUrl;
};

inline void from_json(Json const &json, SrtConnection &value)
{
	json.at("type").get_to(value.type);
	json.at("srtUrl").get_to(value.srtUrl);
	json.at("heartbeatUrl").get_to(value.heartbeatUrl);
	json.at("snapshotUrl").get_to(value.snapshotUrl);
}

struct SrtStream {
	Id id;
	std::string url;
	std::vector<SrtConnection> srtConnection;
};

inline void from_json(Json const &json, SrtStream &value)
{
	json.at("id").get_to(value.id);
	json.at("url").get_to(value.url);
	json.at("srtConnection").get_to(value.srtConnection);
}

struct StageSubscriptionViewerStreamInput {
	Id feedId;
	ViewerStreamInput viewerStream;
};

inline void to_json(Json &json, StageSubscriptionViewerStreamInput const &value)
{
	json["feedId"] = value.feedId;
	json["viewerStream"] = value.viewerStream;
}

struct UnknownStream {
	Id id;
	std::string url;
};

/*
    A stream is a CDN resource for viewing or broadcasting.
    
    After you get a stream you should set up a connection directly to Shepard and
    begin heartbeating there.
    */
struct Stream {
	variant<BroadcasterStream, ViewerStream, SrtStream, UnknownStream>
		implementation;

	Id const &id() const
	{
		return visit(
			[](auto const &implementation) -> Id const & {
				return implementation.id;
			},
			implementation);
	}

	std::string const &url() const
	{
		return visit(
			[](auto const &implementation) -> std::string const & {
				return implementation.url;
			},
			implementation);
	}
};

inline void from_json(Json const &json, UnknownStream &value)
{
	json.at("id").get_to(value.id);
	json.at("url").get_to(value.url);
}

inline void from_json(Json const &json, Stream &value)
{
	std::string occupiedType = json.at("__typename");
	if (occupiedType == "BroadcasterStream") {
		value = {BroadcasterStream(json)};
	} else if (occupiedType == "ViewerStream") {
		value = {ViewerStream(json)};
	} else if (occupiedType == "SrtStream") {
		value = {SrtStream(json)};
	} else {
		value = {UnknownStream(json)};
	}
}

struct Feed {
	Id id;
	Id clientId;
	ClientType clientType;
	// Represents content identifiable in Roadhog's game table.
	optional<Id> gameId;
	SourceConnectionQuality sourceConnectionQuality;
	std::vector<Capability> capabilities;
	/*
        Reyes may override the role a client specifies in order to maintain a valid
        feed / role configuration.
        */
	Role role;
	/*
        Currently, only Garrus may set restrictions, which is why they are not
        included on the FeedInput type.
        
        Restrictions indicate when content is geofenced. If a user tries to view
        geofenced content, it will allocate a special stream that shows a message
        that the content is blocked.
        
        Geofencing is performed based on the client's IP address.
        */
	std::vector<Restriction> restrictions;
	optional<LiveHost> liveHost;
	Stream stream;
};

inline void from_json(Json const &json, Feed &value)
{
	json.at("id").get_to(value.id);
	json.at("clientId").get_to(value.clientId);
	json.at("clientType").get_to(value.clientType);
	{
		auto it = json.find("gameId");
		if (it != json.end()) {
			it->get_to(value.gameId);
		} else {
			value.gameId.reset();
		}
	}
	json.at("sourceConnectionQuality").get_to(value.sourceConnectionQuality);
	json.at("capabilities").get_to(value.capabilities);
	json.at("role").get_to(value.role);
	json.at("restrictions").get_to(value.restrictions);
	{
		auto it = json.find("liveHost");
		if (it != json.end()) {
			it->get_to(value.liveHost);
		} else {
			value.liveHost.reset();
		}
	}
	json.at("stream").get_to(value.stream);
}

struct Stage {
	// The Roadhog account ID of the owner of the stage (no CAID prefix).
	Id id;
	// The username of the owner of the stage.
	std::string username;
	std::string title;
	ContentRating contentRating;
	bool live;
	/*
        If true, then the reactions experience is disabled for this stage, and
        clients should disconnect from the messages websocket.
        */
	bool reactionsDisabled;
	// If true, then replay starts after a live broadcast ends.
	bool autoReplay;
	/*
        We always want to return the HLS URL information for the stage if available.
        
        We are making this optional, if we don't have any HLS URL information available to a stage, we don't return anything.
        
        Note: For now we are planning to do this for URLTV and TheBoy channel on production. For testing, we will support
        Redbull and EdgeSportsTV stage
        */
	optional<std::string> hlsUrl;
	/*
        We always want to return the trailer URL information for the stage if available.
        
        We are making this optional, if we don't have any trailer URL information available to a stage, we don't return anything.
        */
	optional<std::string> trailerUrl;
	/*
        The state of the stage acccording to Reyes and the current user's
        permissions. Clients may ignore this value for now, but it needs to be
        included in the schema in order to be returned with setStageStateAsAdmin.
        */
	StageState state;
	/*
        Indicates which badge should be shown on the stage in the UI. Clients should
        map this value to the appropriate asset.
        */
	StageBadge badge;
	/*
        For now, clients still need to upload their own images.
        
        A broadcast ID will be created when the stage goes from having no feeds to
        having at least one feed.
        
        When a stage goes offline, the broadcastId will be cleared.
        
        This is optional in order to make it easier to deprecate and to ensure that
        it is only set when the stage has a valid broadcast ID.
        
        If you don't have a broadcast ID.
        */
	optional<Id> broadcastId;
	/*
        If the stage is live (or in test mode), then the ControllingClient will be
        populated. This indicates that there exists a client that is in "control" of
        the stage. While there is a controller, other clients will not be able to
        edit the stage unless strictly authorized to do so (according to the client
        takeover rules, documented
        https://github.com/caffeinetv/reyes/blob/master/docs/client-contention.md).
        */
	optional<ControllingClient> controllingClient;
	/*
        viewingOptions lists the ways that this stage can be watched. Clients
        should use these values as candidates for the selectViewingOption mutation.
        */
	std::vector<ViewingOption> viewingOptions;
	/*
        currentViewingOption is the currently selected viewingOption. It will be
        null for a broadcasting client.
        */
	optional<ViewingOption> currentViewingOption;
	/*
        Contains only the feeds that the requesting client should be concerned with.
        (i.e. The feeds necessary to allow the client to view the stage.)
        */
	std::vector<Feed> feeds;
	// Contains the paywall information with ppv show and event details that is set up for the stage.
	optional<Paywall> paywall;
	/*
        Indicates if the current viewer is blocked from viewing the premium show. If false, the clients should show
        the viewer the paywall modal to purchase and get access to the premium show.
        */
	bool unlocked;
};

inline void from_json(Json const &json, Stage &value)
{
	json.at("id").get_to(value.id);
	json.at("username").get_to(value.username);
	json.at("title").get_to(value.title);
	json.at("contentRating").get_to(value.contentRating);
	json.at("live").get_to(value.live);
	json.at("reactionsDisabled").get_to(value.reactionsDisabled);
	json.at("autoReplay").get_to(value.autoReplay);
	{
		auto it = json.find("hlsUrl");
		if (it != json.end()) {
			it->get_to(value.hlsUrl);
		} else {
			value.hlsUrl.reset();
		}
	}
	{
		auto it = json.find("trailerUrl");
		if (it != json.end()) {
			it->get_to(value.trailerUrl);
		} else {
			value.trailerUrl.reset();
		}
	}
	json.at("state").get_to(value.state);
	json.at("badge").get_to(value.badge);
	{
		auto it = json.find("broadcastId");
		if (it != json.end()) {
			it->get_to(value.broadcastId);
		} else {
			value.broadcastId.reset();
		}
	}
	{
		auto it = json.find("controllingClient");
		if (it != json.end()) {
			it->get_to(value.controllingClient);
		} else {
			value.controllingClient.reset();
		}
	}
	json.at("viewingOptions").get_to(value.viewingOptions);
	{
		auto it = json.find("currentViewingOption");
		if (it != json.end()) {
			it->get_to(value.currentViewingOption);
		} else {
			value.currentViewingOption.reset();
		}
	}
	json.at("feeds").get_to(value.feeds);
	{
		auto it = json.find("paywall");
		if (it != json.end()) {
			it->get_to(value.paywall);
		} else {
			value.paywall.reset();
		}
	}
	json.at("unlocked").get_to(value.unlocked);
}

struct StageAsAdminPayload {
	/*
        Eventually we will return a raw view of the entire stage. This is difficult
        to do right now due to assumptions made in the GraphQL layer. For now, just
        return the raw stage state since that's what we care about.
        */
	optional<StageState> rawState;
	optional<ViewingOptionRule> viewingOptionRule;
	/*
        The last time this broadcast when online or null.
        
        RFC3339 format used: 2009-11-10T23:00:00Z
        */
	optional<std::string> onlineAt;
	/*
        The last time this broadcast went offline or null.
        If the stage is currently live, it will be null.
        
        RFC3339 format used: 2009-11-10T23:00:00Z
        */
	optional<std::string> offlineAt;
	/*
        The computed stage should be equivalent to what would be returned in the
        non-admin `stage` query.
        */
	Stage computed;
	std::string trailerTitle;
	bool trailerEnabled;
	/*
        The fallbackHlsUrl must be equal to the hls url that is set for a stage.
        This field is required for admins from BA tool, to have the ability to
        differentiate between a computed `replayURL` that is set on `hlsURL`
        for viewers subscribed to a stage when in replay state.
        */
	optional<std::string> fallbackHlsUrl;
	// The replayData must contain the replay information present for a stage.
	optional<ReplayData> replayData;
	// The paywall premiumVideoUrl that is set on the stage.
	optional<std::string> premiumVideoUrl;
};

inline void from_json(Json const &json, StageAsAdminPayload &value)
{
	{
		auto it = json.find("rawState");
		if (it != json.end()) {
			it->get_to(value.rawState);
		} else {
			value.rawState.reset();
		}
	}
	{
		auto it = json.find("viewingOptionRule");
		if (it != json.end()) {
			it->get_to(value.viewingOptionRule);
		} else {
			value.viewingOptionRule.reset();
		}
	}
	{
		auto it = json.find("onlineAt");
		if (it != json.end()) {
			it->get_to(value.onlineAt);
		} else {
			value.onlineAt.reset();
		}
	}
	{
		auto it = json.find("offlineAt");
		if (it != json.end()) {
			it->get_to(value.offlineAt);
		} else {
			value.offlineAt.reset();
		}
	}
	json.at("computed").get_to(value.computed);
	json.at("trailerTitle").get_to(value.trailerTitle);
	json.at("trailerEnabled").get_to(value.trailerEnabled);
	{
		auto it = json.find("fallbackHlsUrl");
		if (it != json.end()) {
			it->get_to(value.fallbackHlsUrl);
		} else {
			value.fallbackHlsUrl.reset();
		}
	}
	{
		auto it = json.find("replayData");
		if (it != json.end()) {
			it->get_to(value.replayData);
		} else {
			value.replayData.reset();
		}
	}
	{
		auto it = json.find("premiumVideoUrl");
		if (it != json.end()) {
			it->get_to(value.premiumVideoUrl);
		} else {
			value.premiumVideoUrl.reset();
		}
	}
}

struct StageSubscriptionPayload {
	optional<Error> error;
	Stage stage;
};

inline void from_json(Json const &json, StageSubscriptionPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
}

struct StartBroadcastPayload {
	optional<Error> error;
	Stage stage;
};

inline void from_json(Json const &json, StartBroadcastPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
}

struct StopBroadcastPayload {
	optional<Error> error;
	Stage stage;
};

inline void from_json(Json const &json, StopBroadcastPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
}

namespace Subscription {

/*
        Use this subscription to get reactive updates to the stage pushed from Reyes.
        Upon connecting, will return immediately with the client's current view of
        the stage. As a viewer, this is where stream allocation happens.
        
        If Reyes fails to retrieve streams for any feed, it will return the feeds
        (possibly 0) that it was able to retrieve streams for. Reyes will then retry
        a few times. If it is still not able to retrieve streams for every feed, then
        it will return a CdnError.
        
        NOTE Stream allocation is not retried in mutations (addFeed, updateFeed, etc.).
        
        Viewer streams allocated here will be made available to views of the stage
        returned in the mutations above; however, those streams will only be valid
        when returned from the mutation if the client is subscribed to stage updates.
        If the client is not subscribed, then those streams may have expired.
        
        NOTE: You must include a valid credential under the `X-Credential` key in
        your connection parameters when connecting to the websocket.
        */
struct StageField {

	static Operation constexpr operation = Operation::Subscription;

	static Json
	request(Id const &clientId, ClientType clientType,
		optional<std::string> const &clientTypeForMetrics,
		std::string const &username, optional<bool> constrainedBaseline,
		optional<std::vector<StageSubscriptionViewerStreamInput>> const
			&viewerStreams,
		optional<bool> skipStreamAllocation,
		optional<StageSource> stageSource)
	{
		Json query = R"(
                    subscription Stage(
                        $clientId: ID!
                        $clientType: ClientType!
                        $clientTypeForMetrics: String
                        $username: String!
                        $constrainedBaseline: Boolean
                        $viewerStreams: [StageSubscriptionViewerStreamInput!]
                        $skipStreamAllocation: Boolean
                        $stageSource: StageSource
                    ) {
                        stage(
                            clientId: $clientId
                            clientType: $clientType
                            clientTypeForMetrics: $clientTypeForMetrics
                            username: $username
                            constrainedBaseline: $constrainedBaseline
                            viewerStreams: $viewerStreams
                            skipStreamAllocation: $skipStreamAllocation
                            stageSource: $stageSource
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["clientType"] = clientType;
		variables["clientTypeForMetrics"] = clientTypeForMetrics;
		variables["username"] = username;
		variables["constrainedBaseline"] = constrainedBaseline;
		variables["viewerStreams"] = viewerStreams;
		variables["skipStreamAllocation"] = skipStreamAllocation;
		variables["stageSource"] = stageSource;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = StageSubscriptionPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("stage"));
		}
	}
};

} // namespace Subscription

struct UpdateFeedPayload {
	optional<Error> error;
	Stage stage;
	Feed feed;
};

inline void from_json(Json const &json, UpdateFeedPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
	json.at("feed").get_to(value.feed);
}

struct AddFeedPayload {
	optional<Error> error;
	Stage stage;
	Feed feed;
};

inline void from_json(Json const &json, AddFeedPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
	json.at("feed").get_to(value.feed);
}

struct ChangeReplayTitleAsAdminPayload {
	optional<Error> error;
	Stage stage;
};

inline void from_json(Json const &json, ChangeReplayTitleAsAdminPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
}

struct ChangeStageContentRatingPayload {
	optional<Error> error;
	Stage stage;
};

inline void from_json(Json const &json, ChangeStageContentRatingPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
}

struct ChangeStageTitlePayload {
	optional<Error> error;
	Stage stage;
};

inline void from_json(Json const &json, ChangeStageTitlePayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
}

namespace Query {

/*
        NOTE All this query does is to read stage and sanitize it depending on who
        the requester is.
        
        No streams are allocated, and no feeds will be removed in order to show the
        client-specific view.
        
        There may be some stream information if Reyes has stored it, but clients
        should never use this information for anything.
        
        NOTE: All queries and mutations must include a valid credential in the
        X-Credential header.
        */
struct StageField {

	static Operation constexpr operation = Operation::Query;

	static Json request(std::string const &username)
	{
		Json query = R"(
                    query Stage(
                        $username: String!
                    ) {
                        stage(
                            username: $username
                        ) {
                            id
                            username
                            title
                            contentRating
                            live
                            reactionsDisabled
                            autoReplay
                            hlsUrl
                            trailerUrl
                            state
                            badge
                            broadcastId
                            controllingClient {
                                clientId
                                clientType
                            }
                            viewingOptions
                            currentViewingOption
                            feeds {
                                id
                                clientId
                                clientType
                                gameId
                                sourceConnectionQuality
                                capabilities
                                role
                                restrictions
                                liveHost {
                                    __typename
                                    address
                                    ...on LiveHosting {
                                        volume
                                        ownerId
                                        ownerUsername
                                    }
                                }
                                stream {
                                    __typename
                                    id
                                    url
                                    ...on BroadcasterStream {
                                        sdpAnswer
                                    }
                                    ...on ViewerStream {
                                        sdpOffer
                                    }
                                    ...on SrtStream {
                                        srtConnection {
                                            type
                                            srtUrl
                                            heartbeatUrl
                                            snapshotUrl
                                        }
                                    }
                                }
                            }
                            paywall {
                                ppvShowId
                                title
                                price
                                enforced
                                header
                                description
                                previewImagePath
                            }
                            unlocked
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = Stage;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("stage"));
		}
	}
};

/*
        Admin-only stage query.
        
        For partner engineering to monitoring stage state changes, they need to be
        able to see the raw state on the stage struct. Currently, the returned stage
        state is a computed property meant to show the relevant state to a viewing or
        broadcasting client.
        
        This query will allow returning raw information that would be innappropriate
        for viewing or broadcasting clients to use but may be useful for admins to
        see in order to fully understand the state of the stage.
        */
struct StageAsAdminField {

	static Operation constexpr operation = Operation::Query;

	static Json request(std::string const &username)
	{
		Json query = R"(
                    query StageAsAdmin(
                        $username: String!
                    ) {
                        stageAsAdmin(
                            username: $username
                        ) {
                            rawState
                            viewingOptionRule
                            onlineAt
                            offlineAt
                            computed {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            trailerTitle
                            trailerEnabled
                            fallbackHlsUrl
                            replayData {
                                adminReplayHLSURL
                                originalTitle
                                originalDate
                                originalBroadcastId
                                scheduledStartTime
                                scheduledEndTime
                            }
                            premiumVideoUrl
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = StageAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("stageAsAdmin"));
		}
	}
};

// Allow admin to get the stage state.
struct StageStateAsAdminField {

	static Operation constexpr operation = Operation::Query;

	static Json request(std::string const &username)
	{
		Json query = R"(
                    query StageStateAsAdmin(
                        $username: String!
                    ) {
                        stageStateAsAdmin(
                            username: $username
                        ) {
                            raw
                            computed
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = StageStatePayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("stageStateAsAdmin"));
		}
	}
};

// Allow admin to get the metadata needed to upload the trailer videos to s3 for a stage.
struct StageTrailerUploadFieldsField {

	static Operation constexpr operation = Operation::Query;

	static Json request(std::string const &username,
			    std::string const &path,
			    std::string const &contentType)
	{
		Json query = R"(
                    query StageTrailerUploadFields(
                        $username: String!
                        $path: String!
                        $contentType: String!
                    ) {
                        stageTrailerUploadFields(
                            username: $username
                            path: $path
                            contentType: $contentType
                        ) {
                            username
                            url
                            fields {
                                key
                                contentType
                                policy
                                xAmzCredential
                                xAmzAlgorithm
                                xAmzDate
                                xAmzSignature
                                xAmzSessionToken
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["path"] = path;
		variables["contentType"] = contentType;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = StageTrailerUploadPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("stageTrailerUploadFields"));
		}
	}
};

/*
        For Social Share, long/short deep links username(stage username) and channels are required fields.
        imageUrl is optional field where clients can send subset of the imape path.
        For upcoming card share, username(stage username), imageUrl, shareType and message are required fields.
        Clients will pass in the full image link of the upcoming card in imageUrl, shareType as UPCOMING, title
        and the message to set for the upcoming cards.
        */
struct StageShareLinksField {

	static Operation constexpr operation = Operation::Query;

	static Json request(std::string const &username,
			    std::vector<StageShareChannel> const &channels,
			    optional<std::string> const &imageUrl,
			    optional<ShareType> shareType,
			    optional<std::string> const &title,
			    optional<std::string> const &message)
	{
		Json query = R"(
                    query StageShareLinks(
                        $username: String!
                        $channels: [StageShareChannel!]!
                        $imageUrl: String
                        $shareType: ShareType
                        $title: String
                        $message: String
                    ) {
                        stageShareLinks(
                            username: $username
                            channels: $channels
                            imageUrl: $imageUrl
                            shareType: $shareType
                            title: $title
                            message: $message
                        ) {
                            stageShareLinks {
                                channel
                                deepLink
                                inviteeId
                            }
                            message
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["channels"] = channels;
		variables["imageUrl"] = imageUrl;
		variables["shareType"] = shareType;
		variables["title"] = title;
		variables["message"] = message;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = StageShareLinksPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("stageShareLinks"));
		}
	}
};

struct ShareDeepLinksField {

	static Operation constexpr operation = Operation::Query;

	static Json request(optional<InviteLinksInput> const &inviteLinks)
	{
		Json query = R"(
                    query ShareDeepLinks(
                        $inviteLinks: InviteLinksInput
                    ) {
                        shareDeepLinks(
                            inviteLinks: $inviteLinks
                        ) {
                            stageShareLinks {
                                channel
                                deepLink
                                inviteeId
                            }
                            message
                        }
                    }
                )";
		Json variables;
		variables["inviteLinks"] = inviteLinks;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = StageShareLinksPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("shareDeepLinks"));
		}
	}
};

struct SocialShareUploadFieldsField {

	static Operation constexpr operation = Operation::Query;

	static Json request(std::string const &username,
			    std::string const &path,
			    std::string const &contentType)
	{
		Json query = R"(
                    query SocialShareUploadFields(
                        $username: String!
                        $path: String!
                        $contentType: String!
                    ) {
                        socialShareUploadFields(
                            username: $username
                            path: $path
                            contentType: $contentType
                        ) {
                            username
                            url
                            fields {
                                key
                                contentType
                                policy
                                xAmzCredential
                                xAmzAlgorithm
                                xAmzDate
                                xAmzSignature
                                xAmzSessionToken
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["path"] = path;
		variables["contentType"] = contentType;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = S3UploadFieldsPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("socialShareUploadFields"));
		}
	}
};

struct SocialShareFieldsAsAdminField {

	static Operation constexpr operation = Operation::Query;

	static Json request(std::string const &username)
	{
		Json query = R"(
                    query SocialShareFieldsAsAdmin(
                        $username: String!
                    ) {
                        socialShareFieldsAsAdmin(
                            username: $username
                        ) {
                            username
                            ogTitle
                            ogDescription
                            ogImageUrl
                            message
                            error {
                                __typename
                                title
                                message
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SocialShareFieldsAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("socialShareFieldsAsAdmin"));
		}
	}
};

} // namespace Query

struct RemoveFeedPayload {
	optional<Error> error;
	Stage stage;
};

inline void from_json(Json const &json, RemoveFeedPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
}

struct SetAutoReplayAsAdminPayload {
	std::string username;
	bool autoReplay;
	Stage stage;
};

inline void from_json(Json const &json, SetAutoReplayAsAdminPayload &value)
{
	json.at("username").get_to(value.username);
	json.at("autoReplay").get_to(value.autoReplay);
	json.at("stage").get_to(value.stage);
}

struct SetAutoReplayPayload {
	std::string username;
	bool autoReplay;
	Stage stage;
};

inline void from_json(Json const &json, SetAutoReplayPayload &value)
{
	json.at("username").get_to(value.username);
	json.at("autoReplay").get_to(value.autoReplay);
	json.at("stage").get_to(value.stage);
}

struct SetHlsUrlAsAdminPayload {
	Stage stage;
};

inline void from_json(Json const &json, SetHlsUrlAsAdminPayload &value)
{
	json.at("stage").get_to(value.stage);
}

struct SetLiveHostingFeedPayload {
	optional<Error> error;
	Stage stage;
	Feed feed;
};

inline void from_json(Json const &json, SetLiveHostingFeedPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
	json.at("feed").get_to(value.feed);
}

struct SetReactionsDisabledAsAdminPayload {
	optional<Error> error;
	Stage stage;
	bool reactionsDisabled;
};

inline void from_json(Json const &json,
		      SetReactionsDisabledAsAdminPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
	json.at("reactionsDisabled").get_to(value.reactionsDisabled);
}

struct SetStageStateAsAdminPayload {
	optional<Error> error;
	Stage stage;
	optional<StageState> rawState;
};

inline void from_json(Json const &json, SetStageStateAsAdminPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
	{
		auto it = json.find("rawState");
		if (it != json.end()) {
			it->get_to(value.rawState);
		} else {
			value.rawState.reset();
		}
	}
}

struct SetTrailerFlagAsAdminPayload {
	optional<Error> error;
	Stage stage;
	bool trailerEnabled;
	std::string trailerTitle;
};

inline void from_json(Json const &json, SetTrailerFlagAsAdminPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
	json.at("trailerEnabled").get_to(value.trailerEnabled);
	json.at("trailerTitle").get_to(value.trailerTitle);
}

struct SetTrailerTitleAsAdminPayload {
	Stage stage;
	std::string trailerTitle;
	bool trailerEnabled;
};

inline void from_json(Json const &json, SetTrailerTitleAsAdminPayload &value)
{
	json.at("stage").get_to(value.stage);
	json.at("trailerTitle").get_to(value.trailerTitle);
	json.at("trailerEnabled").get_to(value.trailerEnabled);
}

struct SetTrailerUrlAsAdminPayload {
	Stage stage;
	std::string trailerTitle;
	bool trailerEnabled;
};

inline void from_json(Json const &json, SetTrailerUrlAsAdminPayload &value)
{
	json.at("stage").get_to(value.stage);
	json.at("trailerTitle").get_to(value.trailerTitle);
	json.at("trailerEnabled").get_to(value.trailerEnabled);
}

struct SetViewingOptionRuleAsAdminPayload {
	optional<Error> error;
	Stage stage;
	optional<ViewingOptionRule> viewingOptionRule;
};

inline void from_json(Json const &json,
		      SetViewingOptionRuleAsAdminPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
	json.at("stage").get_to(value.stage);
	{
		auto it = json.find("viewingOptionRule");
		if (it != json.end()) {
			it->get_to(value.viewingOptionRule);
		} else {
			value.viewingOptionRule.reset();
		}
	}
}

struct UnknownStagePayload {
	Stage stage;
};

struct StagePayload {
	variant<AddFeedPayload, SetLiveHostingFeedPayload, UpdateFeedPayload,
		RemoveFeedPayload, ChangeStageTitlePayload,
		ChangeReplayTitleAsAdminPayload,
		ChangeStageContentRatingPayload, StartBroadcastPayload,
		SetStageStateAsAdminPayload, SetViewingOptionRuleAsAdminPayload,
		SetReactionsDisabledAsAdminPayload, SetTrailerFlagAsAdminPayload,
		StopBroadcastPayload, SetAutoReplayPayload,
		SetAutoReplayAsAdminPayload, StageSubscriptionPayload,
		SetHlsUrlAsAdminPayload, SetTrailerTitleAsAdminPayload,
		SetTrailerUrlAsAdminPayload, UnknownStagePayload>
		implementation;

	Stage const &stage() const
	{
		return visit(
			[](auto const &implementation) -> Stage const & {
				return implementation.stage;
			},
			implementation);
	}
};

inline void from_json(Json const &json, UnknownStagePayload &value)
{
	json.at("stage").get_to(value.stage);
}

inline void from_json(Json const &json, StagePayload &value)
{
	std::string occupiedType = json.at("__typename");
	if (occupiedType == "AddFeedPayload") {
		value = {AddFeedPayload(json)};
	} else if (occupiedType == "SetLiveHostingFeedPayload") {
		value = {SetLiveHostingFeedPayload(json)};
	} else if (occupiedType == "UpdateFeedPayload") {
		value = {UpdateFeedPayload(json)};
	} else if (occupiedType == "RemoveFeedPayload") {
		value = {RemoveFeedPayload(json)};
	} else if (occupiedType == "ChangeStageTitlePayload") {
		value = {ChangeStageTitlePayload(json)};
	} else if (occupiedType == "ChangeReplayTitleAsAdminPayload") {
		value = {ChangeReplayTitleAsAdminPayload(json)};
	} else if (occupiedType == "ChangeStageContentRatingPayload") {
		value = {ChangeStageContentRatingPayload(json)};
	} else if (occupiedType == "StartBroadcastPayload") {
		value = {StartBroadcastPayload(json)};
	} else if (occupiedType == "SetStageStateAsAdminPayload") {
		value = {SetStageStateAsAdminPayload(json)};
	} else if (occupiedType == "SetViewingOptionRuleAsAdminPayload") {
		value = {SetViewingOptionRuleAsAdminPayload(json)};
	} else if (occupiedType == "SetReactionsDisabledAsAdminPayload") {
		value = {SetReactionsDisabledAsAdminPayload(json)};
	} else if (occupiedType == "SetTrailerFlagAsAdminPayload") {
		value = {SetTrailerFlagAsAdminPayload(json)};
	} else if (occupiedType == "StopBroadcastPayload") {
		value = {StopBroadcastPayload(json)};
	} else if (occupiedType == "SetAutoReplayPayload") {
		value = {SetAutoReplayPayload(json)};
	} else if (occupiedType == "SetAutoReplayAsAdminPayload") {
		value = {SetAutoReplayAsAdminPayload(json)};
	} else if (occupiedType == "StageSubscriptionPayload") {
		value = {StageSubscriptionPayload(json)};
	} else if (occupiedType == "SetHlsUrlAsAdminPayload") {
		value = {SetHlsUrlAsAdminPayload(json)};
	} else if (occupiedType == "SetTrailerTitleAsAdminPayload") {
		value = {SetTrailerTitleAsAdminPayload(json)};
	} else if (occupiedType == "SetTrailerUrlAsAdminPayload") {
		value = {SetTrailerUrlAsAdminPayload(json)};
	} else {
		value = {UnknownStagePayload(json)};
	}
}

struct UnknownErrorPayload {
	optional<Error> error;
};

/*
    If the error is present, then you should not expect the Stage to have
    meaningful information (i.e. it will be a zero value).
    */
struct ErrorPayload {
	variant<AddFeedPayload, SetLiveHostingFeedPayload, UpdateFeedPayload,
		RemoveFeedPayload, ChangeStageTitlePayload,
		ChangeReplayTitleAsAdminPayload,
		ChangeStageContentRatingPayload, StartBroadcastPayload,
		SetStageStateAsAdminPayload, SetViewingOptionRuleAsAdminPayload,
		SetReactionsDisabledAsAdminPayload, SetTrailerFlagAsAdminPayload,
		SocialShareFieldsAsAdminPayload, StopBroadcastPayload,
		StageSubscriptionPayload, UnknownErrorPayload>
		implementation;

	optional<Error> const &error() const
	{
		return visit(
			[](auto const &implementation)
				-> optional<Error> const & {
				return implementation.error;
			},
			implementation);
	}
};

inline void from_json(Json const &json, UnknownErrorPayload &value)
{
	{
		auto it = json.find("error");
		if (it != json.end()) {
			it->get_to(value.error);
		} else {
			value.error.reset();
		}
	}
}

inline void from_json(Json const &json, ErrorPayload &value)
{
	std::string occupiedType = json.at("__typename");
	if (occupiedType == "AddFeedPayload") {
		value = {AddFeedPayload(json)};
	} else if (occupiedType == "SetLiveHostingFeedPayload") {
		value = {SetLiveHostingFeedPayload(json)};
	} else if (occupiedType == "UpdateFeedPayload") {
		value = {UpdateFeedPayload(json)};
	} else if (occupiedType == "RemoveFeedPayload") {
		value = {RemoveFeedPayload(json)};
	} else if (occupiedType == "ChangeStageTitlePayload") {
		value = {ChangeStageTitlePayload(json)};
	} else if (occupiedType == "ChangeReplayTitleAsAdminPayload") {
		value = {ChangeReplayTitleAsAdminPayload(json)};
	} else if (occupiedType == "ChangeStageContentRatingPayload") {
		value = {ChangeStageContentRatingPayload(json)};
	} else if (occupiedType == "StartBroadcastPayload") {
		value = {StartBroadcastPayload(json)};
	} else if (occupiedType == "SetStageStateAsAdminPayload") {
		value = {SetStageStateAsAdminPayload(json)};
	} else if (occupiedType == "SetViewingOptionRuleAsAdminPayload") {
		value = {SetViewingOptionRuleAsAdminPayload(json)};
	} else if (occupiedType == "SetReactionsDisabledAsAdminPayload") {
		value = {SetReactionsDisabledAsAdminPayload(json)};
	} else if (occupiedType == "SetTrailerFlagAsAdminPayload") {
		value = {SetTrailerFlagAsAdminPayload(json)};
	} else if (occupiedType == "SocialShareFieldsAsAdminPayload") {
		value = {SocialShareFieldsAsAdminPayload(json)};
	} else if (occupiedType == "StopBroadcastPayload") {
		value = {StopBroadcastPayload(json)};
	} else if (occupiedType == "StageSubscriptionPayload") {
		value = {StageSubscriptionPayload(json)};
	} else {
		value = {UnknownErrorPayload(json)};
	}
}

namespace Mutation {

/*
        Add a feed to the stage. Any non-required fields in the input will be filled
        with default values if not specified.
        
        Reyes will not allow you to add the feed if another client is controlling the
        stage.
        
        Reyes may limit the number of feeds and live hostable feeds each client is
        allowed to add.
        
        Adding a feed may mutate other feeds, since Reyes always maintains a legal
        assignment of roles to the various feeds.
        
        Roles are assigned according to the following rules:
        
        1. Each client may have at most 2 feeds on the stage.
        2. Each client may have at most 1 live hosting feed on the stage.
        3. If the client did not specify the roles on the feeds, then the last feed
        that was added will be PRIMARY.
        4. However, if one of the feeds is live hosting, then that feed will be made
        PRIMARY.
        
        Returns a valid view of the stage for that client, including any necessary
        Broadcaster and Viewer streams (when live hosting).
        */
struct AddFeedField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, ClientType clientType,
			    FeedInput const &input)
	{
		Json query = R"(
                    mutation AddFeed(
                        $clientId: ID!
                        $clientType: ClientType!
                        $input: FeedInput!
                    ) {
                        addFeed(
                            clientId: $clientId
                            clientType: $clientType
                            input: $input
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            feed {
                                id
                                clientId
                                clientType
                                gameId
                                sourceConnectionQuality
                                capabilities
                                role
                                restrictions
                                liveHost {
                                    __typename
                                    address
                                    ...on LiveHosting {
                                        volume
                                        ownerId
                                        ownerUsername
                                    }
                                }
                                stream {
                                    __typename
                                    id
                                    url
                                    ...on BroadcasterStream {
                                        sdpAnswer
                                    }
                                    ...on ViewerStream {
                                        sdpOffer
                                    }
                                    ...on SrtStream {
                                        srtConnection {
                                            type
                                            srtUrl
                                            heartbeatUrl
                                            snapshotUrl
                                        }
                                    }
                                }
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["clientType"] = clientType;
		variables["input"] = input;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = AddFeedPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("addFeed"));
		}
	}
};

/*
        Sets the live hosting feed to the stage. If a live hosting feed was already on
        the stage, this mutation will remove the pre-existing feed.
        
        If you try to set a non-hostable feed, then you will receive an error.
        
        You may also add live hosting feeds using the regular addFeed mutation, but
        doing so will give an error if you attempt to add more than a legal number of
        live hosting feeds to the stage.
        
        NOTE Stream allocation will not be retried if it fails. Instead, adding the
        feed will fail with a CdnError. This is also true for the other mutations.
        
        Returns a valid view of the stage for that client, including any necessary
        Broadcaster and Viewer streams (when live hosting).
        */
struct SetLiveHostingFeedField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, ClientType clientType,
			    FeedInput const &input)
	{
		Json query = R"(
                    mutation SetLiveHostingFeed(
                        $clientId: ID!
                        $clientType: ClientType!
                        $input: FeedInput!
                    ) {
                        setLiveHostingFeed(
                            clientId: $clientId
                            clientType: $clientType
                            input: $input
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            feed {
                                id
                                clientId
                                clientType
                                gameId
                                sourceConnectionQuality
                                capabilities
                                role
                                restrictions
                                liveHost {
                                    __typename
                                    address
                                    ...on LiveHosting {
                                        volume
                                        ownerId
                                        ownerUsername
                                    }
                                }
                                stream {
                                    __typename
                                    id
                                    url
                                    ...on BroadcasterStream {
                                        sdpAnswer
                                    }
                                    ...on ViewerStream {
                                        sdpOffer
                                    }
                                    ...on SrtStream {
                                        srtConnection {
                                            type
                                            srtUrl
                                            heartbeatUrl
                                            snapshotUrl
                                        }
                                    }
                                }
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["clientType"] = clientType;
		variables["input"] = input;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetLiveHostingFeedPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("setLiveHostingFeed"));
		}
	}
};

/*
        Reyes will only modify fields that are explicitly set in the input.
        
        It will give an error of the specified feed does not exist.
        
        Returns a valid view of the stage for that client, including any necessary
        Broadcaster and Viewer streams (when live hosting).
        */
struct UpdateFeedField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, ClientType clientType,
			    FeedInput const &input)
	{
		Json query = R"(
                    mutation UpdateFeed(
                        $clientId: ID!
                        $clientType: ClientType!
                        $input: FeedInput!
                    ) {
                        updateFeed(
                            clientId: $clientId
                            clientType: $clientType
                            input: $input
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            feed {
                                id
                                clientId
                                clientType
                                gameId
                                sourceConnectionQuality
                                capabilities
                                role
                                restrictions
                                liveHost {
                                    __typename
                                    address
                                    ...on LiveHosting {
                                        volume
                                        ownerId
                                        ownerUsername
                                    }
                                }
                                stream {
                                    __typename
                                    id
                                    url
                                    ...on BroadcasterStream {
                                        sdpAnswer
                                    }
                                    ...on ViewerStream {
                                        sdpOffer
                                    }
                                    ...on SrtStream {
                                        srtConnection {
                                            type
                                            srtUrl
                                            heartbeatUrl
                                            snapshotUrl
                                        }
                                    }
                                }
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["clientType"] = clientType;
		variables["input"] = input;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = UpdateFeedPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("updateFeed"));
		}
	}
};

/*
        This mutation will give you an error if you try to remove the last feed on a
        live stage.
        
        It will also give an error of the specified feed does not exist.
        */
struct RemoveFeedField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, ClientType clientType,
			    Id const &feedId)
	{
		Json query = R"(
                    mutation RemoveFeed(
                        $clientId: ID!
                        $clientType: ClientType!
                        $feedId: ID!
                    ) {
                        removeFeed(
                            clientId: $clientId
                            clientType: $clientType
                            feedId: $feedId
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["clientType"] = clientType;
		variables["feedId"] = feedId;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = RemoveFeedPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("removeFeed"));
		}
	}
};

// If a client is controlling the stage, then only that client may change the title.
struct ChangeStageTitleField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, ClientType clientType,
			    std::string const &title)
	{
		Json query = R"(
                    mutation ChangeStageTitle(
                        $clientId: ID!
                        $clientType: ClientType!
                        $title: String!
                    ) {
                        changeStageTitle(
                            clientId: $clientId
                            clientType: $clientType
                            title: $title
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["clientType"] = clientType;
		variables["title"] = title;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = ChangeStageTitlePayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("changeStageTitle"));
		}
	}
};

// From BA tool, admin can request to update replay title on a stage, when the stage is in replay state.
struct ChangeReplayTitleAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username,
			    std::string const &title)
	{
		Json query = R"(
                    mutation ChangeReplayTitleAsAdmin(
                        $username: String!
                        $title: String!
                    ) {
                        changeReplayTitleAsAdmin(
                            username: $username
                            title: $title
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["title"] = title;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = ChangeReplayTitleAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("changeReplayTitleAsAdmin"));
		}
	}
};

// If a client is controlling the stage, then only that client may change content rating.
struct ChangeStageContentRatingField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, ClientType clientType,
			    ContentRating contentRating)
	{
		Json query = R"(
                    mutation ChangeStageContentRating(
                        $clientId: ID!
                        $clientType: ClientType!
                        $contentRating: ContentRating!
                    ) {
                        changeStageContentRating(
                            clientId: $clientId
                            clientType: $clientType
                            contentRating: $contentRating
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["clientType"] = clientType;
		variables["contentRating"] = contentRating;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = ChangeStageContentRatingPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("changeStageContentRating"));
		}
	}
};

/*
        Set's auto replay toggle on a stage.
        If auto replay is on, then the replay is played on the stage when
        the creator is looking to end the live broadcast. Note that, when the
        creator is looking to end the broadcast, the stage transitions from
        live->replay state without stage going down.
        
        If auto replay is turned off, and the stage is on replay state, then
        the replay is taken off the stage, otherwise then replay is not turned
        on after a creator broadcast ends.
        */
struct SetAutoReplayField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username, bool autoReplay)
	{
		Json query = R"(
                    mutation SetAutoReplay(
                        $username: String!
                        $autoReplay: Boolean!
                    ) {
                        setAutoReplay(
                            username: $username
                            autoReplay: $autoReplay
                        ) {
                            username
                            autoReplay
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["autoReplay"] = autoReplay;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetAutoReplayPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("setAutoReplay"));
		}
	}
};

struct SetAutoReplayAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username, bool autoReplay)
	{
		Json query = R"(
                    mutation SetAutoReplayAsAdmin(
                        $username: String!
                        $autoReplay: Boolean!
                    ) {
                        setAutoReplayAsAdmin(
                            username: $username
                            autoReplay: $autoReplay
                        ) {
                            username
                            autoReplay
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["autoReplay"] = autoReplay;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetAutoReplayAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("setAutoReplayAsAdmin"));
		}
	}
};

/*
        If multiple clients were in preview mode and had feeds on the stage, starting
        a broadcast will remove the feeds no longer in use for the live broadcast.
        
        startBroadcast optionally takes in socialMetadata as input
        */
struct StartBroadcastField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, ClientType clientType,
			    optional<std::string> const &title,
			    optional<SocialMetadataInput> const &socialMetadata)
	{
		Json query = R"(
                    mutation StartBroadcast(
                        $clientId: ID!
                        $clientType: ClientType!
                        $title: String
                        $socialMetadata: SocialMetadataInput
                    ) {
                        startBroadcast(
                            clientId: $clientId
                            clientType: $clientType
                            title: $title
                            socialMetadata: $socialMetadata
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["clientType"] = clientType;
		variables["title"] = title;
		variables["socialMetadata"] = socialMetadata;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = StartBroadcastPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("startBroadcast"));
		}
	}
};

/*
        keepLiveHostingFeeds, if true, will leave any live hosting feeds on the stage.
        Otherwise, all other feeds will be removed.
        */
struct StopBroadcastField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId,
			    optional<bool> keepLiveHostingFeeds)
	{
		Json query = R"(
                    mutation StopBroadcast(
                        $clientId: ID!
                        $keepLiveHostingFeeds: Boolean
                    ) {
                        stopBroadcast(
                            clientId: $clientId
                            keepLiveHostingFeeds: $keepLiveHostingFeeds
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["keepLiveHostingFeeds"] = keepLiveHostingFeeds;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = StopBroadcastPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("stopBroadcast"));
		}
	}
};

/*
        As an admin, set the state of the stage. If the state is `null`, then it will
        be cleared on the stage.
        */
struct SetStageStateAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username,
			    optional<StageState> state)
	{
		Json query = R"(
                    mutation SetStageStateAsAdmin(
                        $username: String!
                        $state: StageState
                    ) {
                        setStageStateAsAdmin(
                            username: $username
                            state: $state
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            rawState
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["state"] = state;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetStageStateAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("setStageStateAsAdmin"));
		}
	}
};

/*
        As an admin, set the viewing option rule. If the rule is set to `null`,
        then it will be clared.
        */
struct SetViewingOptionRuleAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username,
			    optional<ViewingOptionRule> rule)
	{
		Json query = R"(
                    mutation SetViewingOptionRuleAsAdmin(
                        $username: String!
                        $rule: ViewingOptionRule
                    ) {
                        setViewingOptionRuleAsAdmin(
                            username: $username
                            rule: $rule
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            viewingOptionRule
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["rule"] = rule;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetViewingOptionRuleAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("setViewingOptionRuleAsAdmin"));
		}
	}
};

// As an admin, set whether or not reactions are disabled for the given stage.
struct SetReactionsDisabledAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username, bool disabled)
	{
		Json query = R"(
                    mutation SetReactionsDisabledAsAdmin(
                        $username: String!
                        $disabled: Boolean!
                    ) {
                        setReactionsDisabledAsAdmin(
                            username: $username
                            disabled: $disabled
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            reactionsDisabled
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["disabled"] = disabled;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetReactionsDisabledAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("setReactionsDisabledAsAdmin"));
		}
	}
};

/*
        Use this when the CDN gives you a 400 type error on the stream to tell Reyes
        you need a new viewer stream.
        
        (500's should be retried)
        
        This request will return immediately and trigger an update in the stage
        subscription with the new stream.
        */
struct RequestViewerStreamField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, Id const &feedId)
	{
		Json query = R"(
                    mutation RequestViewerStream(
                        $clientId: ID!
                        $feedId: ID!
                    ) {
                        requestViewerStream(
                            clientId: $clientId
                            feedId: $feedId
                        ) {
                            feedId
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["feedId"] = feedId;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = RequestViewerStreamPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("requestViewerStream"));
		}
	}
};

/*
        selectViewingOption will attempt to switch the viewer's connection to the
        specified ViewingOption. It will return an error if the ViewingOption is
        not currently supported for the stage (for example, if there is no HLS URL
        or if HLS is required).
        
        This request will return immediately and trigger an update in the stage
        subscription with a new view of the stage.
        
        NOTE: Broadcasters will *never* be able to select a viewing option, but
        this mutation will *not* return an error if they try to do so. Instead, the
        currentViewingOption returned from the server will always be `null`.
        */
struct SelectViewingOptionField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(Id const &clientId, ViewingOption viewingOption)
	{
		Json query = R"(
                    mutation SelectViewingOption(
                        $clientId: ID!
                        $viewingOption: ViewingOption!
                    ) {
                        selectViewingOption(
                            clientId: $clientId
                            viewingOption: $viewingOption
                        ) {
                            viewingOption
                        }
                    }
                )";
		Json variables;
		variables["clientId"] = clientId;
		variables["viewingOption"] = viewingOption;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SelectViewingOptionPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("selectViewingOption"));
		}
	}
};

// Admin-only mutation to stop all currently running replays except for allow list
struct TakedownAllReplaysAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request()
	{
		Json query = R"(
                    mutation TakedownAllReplaysAsAdmin(
                    ) {
                        takedownAllReplaysAsAdmin {
                            success
                        }
                    }
                )";
		Json variables;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = TakedownAllReplaysAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("takedownAllReplaysAsAdmin"));
		}
	}
};

// This is an admin-only mutation.
struct TakedownStageField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username)
	{
		Json query = R"(
                    mutation TakedownStage(
                        $username: String!
                    ) {
                        takedownStage(
                            username: $username
                        ) {
                            success
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = TakedownStagePayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("takedownStage"));
		}
	}
};

/*
        As an admin, set the HLS url for a stage.
        If the url is `null`, then it will clear the information associated to the stage
        */
struct SetHlsUrlAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username,
			    optional<std::string> const &url)
	{
		Json query = R"(
                    mutation SetHlsUrlAsAdmin(
                        $username: String!
                        $url: String
                    ) {
                        setHlsUrlAsAdmin(
                            username: $username
                            url: $url
                        ) {
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["url"] = url;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetHlsUrlAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("setHlsUrlAsAdmin"));
		}
	}
};

/*
        As an admin, set the replay url for a stage.
        If the input is `null`, then it will clear the information associated to the stage
        If replayURL is empty, then cancel any replays associated to the stage.
        */
struct SetReplayUrlAndReplayScheduleAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username,
			    optional<ReplayInput> const &replay)
	{
		Json query = R"(
                    mutation SetReplayUrlAndReplayScheduleAsAdmin(
                        $username: String!
                        $replay: ReplayInput
                    ) {
                        setReplayUrlAndReplayScheduleAsAdmin(
                            username: $username
                            replay: $replay
                        ) {
                            originalTitle
                            originalBroadcastDate
                            originalBroadcastId
                            scheduledStartTime
                            scheduledEndTime
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["replay"] = replay;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetReplayUrlAndReplaySchedulePayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at(
				"setReplayUrlAndReplayScheduleAsAdmin"));
		}
	}
};

// As an admin, set the trailer title for a stage.
struct SetStageTrailerTitleAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username,
			    std::string const &trailerTitle)
	{
		Json query = R"(
                    mutation SetStageTrailerTitleAsAdmin(
                        $username: String!
                        $trailerTitle: String!
                    ) {
                        setStageTrailerTitleAsAdmin(
                            username: $username
                            trailerTitle: $trailerTitle
                        ) {
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            trailerTitle
                            trailerEnabled
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["trailerTitle"] = trailerTitle;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetTrailerTitleAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("setStageTrailerTitleAsAdmin"));
		}
	}
};

// As an admin, set whether or not trailer is enabled for the given stage.
struct SetStageTrailerEnabledFlagAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username, bool trailerEnabled)
	{
		Json query = R"(
                    mutation SetStageTrailerEnabledFlagAsAdmin(
                        $username: String!
                        $trailerEnabled: Boolean!
                    ) {
                        setStageTrailerEnabledFlagAsAdmin(
                            username: $username
                            trailerEnabled: $trailerEnabled
                        ) {
                            error {
                                __typename
                                title
                                message
                            }
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            trailerEnabled
                            trailerTitle
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["trailerEnabled"] = trailerEnabled;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetTrailerFlagAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("setStageTrailerEnabledFlagAsAdmin"));
		}
	}
};

/*
        As an admin, set the trailer url for a stage.
        If the url is `null`, then it will clear the information associated to the stage
        */
struct SetTrailerUrlAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username, std::string const &url)
	{
		Json query = R"(
                    mutation SetTrailerUrlAsAdmin(
                        $username: String!
                        $url: String!
                    ) {
                        setTrailerUrlAsAdmin(
                            username: $username
                            url: $url
                        ) {
                            stage {
                                id
                                username
                                title
                                contentRating
                                live
                                reactionsDisabled
                                autoReplay
                                hlsUrl
                                trailerUrl
                                state
                                badge
                                broadcastId
                                controllingClient {
                                    clientId
                                    clientType
                                }
                                viewingOptions
                                currentViewingOption
                                feeds {
                                    id
                                    clientId
                                    clientType
                                    gameId
                                    sourceConnectionQuality
                                    capabilities
                                    role
                                    restrictions
                                    liveHost {
                                        __typename
                                        address
                                        ...on LiveHosting {
                                            volume
                                            ownerId
                                            ownerUsername
                                        }
                                    }
                                    stream {
                                        __typename
                                        id
                                        url
                                        ...on BroadcasterStream {
                                            sdpAnswer
                                        }
                                        ...on ViewerStream {
                                            sdpOffer
                                        }
                                        ...on SrtStream {
                                            srtConnection {
                                                type
                                                srtUrl
                                                heartbeatUrl
                                                snapshotUrl
                                            }
                                        }
                                    }
                                }
                                paywall {
                                    ppvShowId
                                    title
                                    price
                                    enforced
                                    header
                                    description
                                    previewImagePath
                                }
                                unlocked
                            }
                            trailerTitle
                            trailerEnabled
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["url"] = url;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SetTrailerUrlAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(data.at("setTrailerUrlAsAdmin"));
		}
	}
};

// As an admin, set the social share fields.
struct SetSocialShareFieldsAsAdminField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username,
			    SocialShareFieldsInput const &input)
	{
		Json query = R"(
                    mutation SetSocialShareFieldsAsAdmin(
                        $username: String!
                        $input: SocialShareFieldsInput!
                    ) {
                        setSocialShareFieldsAsAdmin(
                            username: $username
                            input: $input
                        ) {
                            username
                            ogTitle
                            ogDescription
                            ogImageUrl
                            message
                            error {
                                __typename
                                title
                                message
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["input"] = input;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = SocialShareFieldsAsAdminPayload;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			return ResponseData(
				data.at("setSocialShareFieldsAsAdmin"));
		}
	}
};

// As an admin, set the paywall information for a given stage.
struct SetPaywallOptionsField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(optional<PaywallInput> const &input)
	{
		Json query = R"(
                    mutation SetPaywallOptions(
                        $input: PaywallInput
                    ) {
                        setPaywallOptions(
                            input: $input
                        ) {
                            success
                            username
                            premiumVideoUrl
                            paywall {
                                ppvShowId
                                title
                                price
                                enforced
                                header
                                description
                                previewImagePath
                            }
                            error {
                                __typename
                                title
                                message
                            }
                        }
                    }
                )";
		Json variables;
		variables["input"] = input;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = optional<PaywallPayload>;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			auto it = data.find("setPaywallOptions");
			if (it != data.end()) {
				return ResponseData(*it);
			} else {
				return ResponseData{};
			}
		}
	}
};

// As an admin, clear the paywall information for a given stage.
struct ClearPaywallField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username)
	{
		Json query = R"(
                    mutation ClearPaywall(
                        $username: String!
                    ) {
                        clearPaywall(
                            username: $username
                        ) {
                            success
                            username
                            premiumVideoUrl
                            paywall {
                                ppvShowId
                                title
                                price
                                enforced
                                header
                                description
                                previewImagePath
                            }
                            error {
                                __typename
                                title
                                message
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = optional<PaywallPayload>;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			auto it = data.find("clearPaywall");
			if (it != data.end()) {
				return ResponseData(*it);
			} else {
				return ResponseData{};
			}
		}
	}
};

// As an admin, have the ability to turn paywall on/off for a given stage.
struct TogglePaywallField {

	static Operation constexpr operation = Operation::Mutation;

	static Json request(std::string const &username, bool enforced)
	{
		Json query = R"(
                    mutation TogglePaywall(
                        $username: String!
                        $enforced: Boolean!
                    ) {
                        togglePaywall(
                            username: $username
                            enforced: $enforced
                        ) {
                            success
                            username
                            premiumVideoUrl
                            paywall {
                                ppvShowId
                                title
                                price
                                enforced
                                header
                                description
                                previewImagePath
                            }
                            error {
                                __typename
                                title
                                message
                            }
                        }
                    }
                )";
		Json variables;
		variables["username"] = username;
		variables["enforced"] = enforced;
		return {{"query", std::move(query)},
			{"variables", std::move(variables)}};
	}

	using ResponseData = optional<PaywallPayload>;

	static GraphqlResponse<ResponseData> response(Json const &json)
	{
		auto errors = json.find("errors");
		if (errors != json.end()) {
			std::vector<GraphqlError> errorsList = *errors;
			return errorsList;
		} else {
			auto const &data = json.at("data");
			auto it = data.find("togglePaywall");
			if (it != data.end()) {
				return ResponseData(*it);
			} else {
				return ResponseData{};
			}
		}
	}
};

} // namespace Mutation

} // namespace caffql
