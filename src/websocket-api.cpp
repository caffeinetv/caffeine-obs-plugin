#include "websocket-api.hpp"
#include "error-logging.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <wincrypt.h>
#elif defined(__APPLE__)
#include <Security/Security.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace caff {
class LoggerStream : public std::ostream, public std::streambuf {
public:
	LoggerStream() : std::ostream(this) {}

protected:
	virtual int overflow(int c) override
	{
		if (c == '\n') {
			CAFF_LOG_DEBUG("[WebsocketClient] %s", buffer.c_str());
			buffer.clear();
		} else if (c != '\r') {
			buffer += static_cast<char>(c);
		}
		return 0;
	}

private:
	std::string buffer;
};

static LoggerStream logStream;

WebsocketClient::WebsocketClient()
{
	client_.set_access_channels(websocketpp::log::alevel::all);
	client_.clear_access_channels(websocketpp::log::alevel::frame_payload);
	client_.set_error_channels(websocketpp::log::elevel::all);
	client_.get_alog().set_ostream(&logStream);
	client_.get_elog().set_ostream(&logStream);

	client_.set_tls_init_handler(
		[this](websocketpp::connection_hdl connection)
			-> std::shared_ptr<websocketpp::lib::asio::ssl::context> {
			auto context = std::make_shared<asio::ssl::context>(
				asio::ssl::context::sslv23);
			context->set_verify_mode(asio::ssl::verify_peer);
			X509_STORE *store = storeCert();
			// Attach X509_STORE to current SSL context;
			SSL_CTX_set_cert_store(
				(SSL_CTX *)context->native_handle(), store);
			return context;
		});

	client_.init_asio();
	client_.start_perpetual();
	clientThread_ = std::thread(&Client::run, &client_);
}

X509_STORE *WebsocketClient::storeCert()
{
#if defined(_WIN32) || defined(_WIN64)
	HCERTSTORE hCertStore = CertOpenSystemStore((HCRYPTPROV)NULL, L"ROOT");
	if (hCertStore == nullptr) {
		CAFF_LOG_ERROR("CertOpenSystemStore failed error %d",
			       GetLastError());
	}

	X509_STORE *store = X509_STORE_new();
	PCCERT_CONTEXT pCertContext = nullptr;
	while ((pCertContext = CertEnumCertificatesInStore(
			hCertStore, pCertContext)) != nullptr) {
		const unsigned char *cert_data = pCertContext->pbCertEncoded;

		X509 *cert =
			d2i_X509(NULL, &cert_data, pCertContext->cbCertEncoded);
		if (cert != nullptr) {
			X509_STORE_add_cert(store, cert);
			X509_free(cert);
		}
	}
	CertFreeCertificateContext(pCertContext);
	CertCloseStore(hCertStore, 0);
	return store;
#elif defined(__APPLE__)
	SecKeychainRef keychain_ref;

	if (SecKeychainOpen(
		    "/System/Library/Keychains/SystemRootCertificates.keychain",
		    &keychain_ref) != errSecSuccess) {
		CAFF_LOG_ERROR("Failed to open Keychain");
	}
	X509_STORE *store = X509_STORE_new();
	CFMutableDictionaryRef search_settings_ref;
	search_settings_ref = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
	CFDictionarySetValue(search_settings_ref, kSecClass,
			     kSecClassCertificate);
	CFDictionarySetValue(search_settings_ref, kSecMatchLimit,
			     kSecMatchLimitAll);
	CFDictionarySetValue(search_settings_ref, kSecReturnRef,
			     kCFBooleanTrue);
	CFDictionarySetValue(search_settings_ref, kSecMatchSearchList,
			     CFArrayCreate(NULL, (const void **)&keychain_ref,
					   1, NULL));
	CFArrayRef result_ref;

	if (SecItemCopyMatching(search_settings_ref,
				(CFTypeRef *)&result_ref) != errSecSuccess) {
		CAFF_LOG_ERROR("SecItemCopyMatching error");
	}

	for (CFIndex i = 0; i < CFArrayGetCount(result_ref); i++) {
		SecCertificateRef item_ref =
			(SecCertificateRef)CFArrayGetValueAtIndex(result_ref,
								  i);
		CFDataRef data_ref;

		if ((data_ref = SecCertificateCopyData(item_ref))) {
			const unsigned char *cert_data =
				CFDataGetBytePtr(data_ref);

			X509 *cert = d2i_X509(NULL, &cert_data,
					      CFDataGetLength(data_ref));
			if (cert != nullptr) {
				X509_STORE_add_cert(store, cert);
				X509_free(cert);
			}
			CFRelease(data_ref);
		}
	}
	CFRelease(keychain_ref);
	return store;

#endif
}

WebsocketClient::~WebsocketClient()
{
	client_.stop_perpetual();
	clientThread_.join();
}

static std::string websocketLogPrefix(std::string const &label)
{
	return "[Websocket " + label + "]";
}

std::optional<WebsocketClient::Connection> WebsocketClient::connect(
	std::string url, std::string label,
	std::function<void(Connection)> openedCallback,
	std::function<void(Connection, ConnectionEndType)> endedCallback,
	std::function<void(Connection, std::string const &)>
		messageReceivedCallback)
{
	std::error_code error;
	auto clientConnection = client_.get_connection(url, error);

	auto logPrefix = websocketLogPrefix(label);

	if (error) {
		CAFF_LOG_ERROR("%s connection initialization error: %s",
			       logPrefix.c_str(), error.message().c_str());
		return {};
	}

	clientConnection->set_open_handler(
		[=](websocketpp::connection_hdl handle) {
			CAFF_LOG_INFO("%s opened", logPrefix.c_str());
			if (openedCallback) {
				openedCallback({label, handle});
			}
		});

	clientConnection->set_close_handler([=](websocketpp::connection_hdl
							handle) {
		CAFF_LOG_INFO("%s closed", logPrefix.c_str());
		std::error_code error;
		if (auto connection = client_.get_con_from_hdl(handle, error)) {
			if (auto connectionError = connection->get_ec()) {
				CAFF_LOG_ERROR(
					"%s close reason: %s",
					logPrefix.c_str(),
					connectionError.message().c_str());
			}
		}

		if (endedCallback) {
			endedCallback({label, handle},
				      ConnectionEndType::Closed);
		}
	});

	clientConnection->set_fail_handler([=](websocketpp::connection_hdl
						       handle) {
		CAFF_LOG_ERROR("%s failed", logPrefix.c_str());
		std::error_code error;
		if (auto connection = client_.get_con_from_hdl(handle, error)) {
			if (auto connectionError = connection->get_ec()) {
				CAFF_LOG_ERROR(
					"%s failure reason: %s",
					logPrefix.c_str(),
					connectionError.message().c_str());
			}
		}

		if (endedCallback) {
			endedCallback({label, handle},
				      ConnectionEndType::Failed);
		}
	});

	clientConnection->set_message_handler(
		[=](websocketpp::connection_hdl handle,
		    Client::message_ptr message) {
			CAFF_LOG_DEBUG("%s message received: %s",
				       logPrefix.c_str(),
				       message->get_payload().c_str());
			if (messageReceivedCallback) {
				messageReceivedCallback({label, handle},
							message->get_payload());
			}
		});

	client_.connect(clientConnection);

	return Connection{label, clientConnection->get_handle()};
}

void WebsocketClient::sendMessage(Connection const &connection,
				  std::string const &message)
{
	std::error_code error;
	client_.send(connection.handle, message,
		     websocketpp::frame::opcode::text, error);
	if (error) {
		CAFF_LOG_ERROR("%s send message error: %s",
			       websocketLogPrefix(connection.label).c_str(),
			       error.message().c_str());
	}
}

void WebsocketClient::close(Connection &&connection)
{
	std::error_code error;
	client_.close(connection.handle, websocketpp::close::status::normal, "",
		      error);
	if (error) {
		CAFF_LOG_ERROR("%s connection close error: %s",
			       websocketLogPrefix(connection.label).c_str(),
			       error.message().c_str());
	}
}

} // namespace caff