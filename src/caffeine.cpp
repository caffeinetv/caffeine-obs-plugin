#include "caffeine.hpp"
#include "error-logging.hpp"

#define CATCHALL_RETURN(ret)                                      \
	catch (std::logic_error ex)                               \
	{                                                         \
		CAFF_LOG_ERROR("Logic error. See logs.");         \
		return ret;                                       \
	}                                                         \
	catch (...)                                               \
	{                                                         \
		CAFF_LOG_ERROR("Unhandled exception. See logs."); \
		return ret;                                       \
	}

char const *sessionAuthResponseString(SessionAuthResponse result)
{
	switch (result) {
	case Success:
		return "Success";
	case Failure:
		return "Failure";

	case UsernameRequired:
		return "Username required";
	case PasswordRequired:
		return "Password required";
	case RefreshTokenRequired:
		return "Refresh token required";
	case InfoIncorrect:
		return "Incorrect signin info";
	case LegalAcceptanceRequired:
		return "User must accept Terms of Service";
	case EmailVerificationRequired:
		return "User must verify email address";
	case MfaOtpRequired:
		return "One-time password required";
	case MfaOtpIncorrect:
		return "One-time password incorrect";

	case OldVersion:
		return "Old version";
	case NotSignedIn:
		return "Not signed in";
	case OutOfCapacity:
		return "Out of capacity";
	case Takeover:
		return "Broadcast takeover";
	case AlreadyBroadcasting:
		return "Already broadcasting";
	case Disconnected:
		return "Disconnected from server";
	case BroadcastFailed:
		return "Broadcast failed";
	case InternetDisconnected:
		return "Internet Connection Lost";
	case CaffeineUnreachable:
		return "Caffeine unreachable";
	case TooManyRequests:
		return "Too many request. Please try again later.";
	case StageSubscriptionTimeOut:
		return "Timed out waiting for stage subscription to open";
	default:
		return "Unknown error";
	}
}
