#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Status results and errors returned API functions
typedef enum SessionAuthResponse {
	Success = 0, // General success result
	Failure,     // General failure result

	// Auth
	UsernameRequired,     // Username is missing
	PasswordRequired,     // Password is missing
	RefreshTokenRequired, // Refresh token is missing
	InfoIncorrect, // Username/password or refresh token are incorrect
	LegalAcceptanceRequired,   // User must accept Caffeine Terms of Use
	EmailVerificationRequired, // User must verify email address
	MfaOtpRequired, // User must enter a 2-factor one-time-password sent to their email
	MfaOtpIncorrect, // The one-time-password provided was incorrect
	TooManyRequests, // Too many requests

	// Broadcast
	OldVersion, // libcaffeine or client version is too old to broadcast successfully
	NotSignedIn, // The instance must be signed in
	OutOfCapacity, // Could not find capacity on the server to start a broadcast
	Takeover, // The user ended the broadcast from another device or instance
	AlreadyBroadcasting,      // A broadcast is already online
	Disconnected,             // Broadcast has been disconnected
	InternetDisconnected,     // Internet Connection Lost
	CaffeineUnreachable,      // Caffeine Unreachable
	StageSubscriptionTimeOut, // Timed out waiting for stage subscription open
	BroadcastFailed,          // Broadcast failed to start

	// Used for bounds checking
	ResultLast = BroadcastFailed
} SessionAuthResponse;

char const *sessionAuthResponseString(SessionAuthResponse result);

// Indicator of connection quality, reported by Caffeine's back-end
typedef enum ConnectionQuality {
	ConnectionQualityGood, // The connection quality is good
	ConnectionQualityPoor, // There is a significant amount of packet loss on the way to the server
	ConnectionQualityUnknown // Connection quality has not been established
} ConnectionQuality;
