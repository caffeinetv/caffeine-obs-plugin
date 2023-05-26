#pragma once

#include "rest-api.hpp"
#include "urls.hpp"

#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"

namespace caff {

class WebsocketClient {

public:
	WebsocketClient();
	~WebsocketClient();

	struct Connection {
		std::string label;
		websocketpp::connection_hdl handle;
	};

	enum class ConnectionEndType { Failed, Closed };

	std::optional<Connection>
	connect(std::string url, std::string label,
		std::function<void(Connection)> openedCallback,
		std::function<void(Connection, ConnectionEndType)> endedCallback,
		std::function<void(Connection, std::string const &)>
			messageReceivedCallback);

	void sendMessage(Connection const &connection,
			 std::string const &message);

	void close(Connection &&connection);

private:
	using Client =
		websocketpp::client<websocketpp::config::asio_tls_client>;
	Client client_;
	std::thread clientThread_;
	X509_STORE *storeCert();
};

template<typename OperationField>
class GraphqlSubscription : public std::enable_shared_from_this<
				    GraphqlSubscription<OperationField>> {
public:
	static_assert(
		OperationField::operation == caffql::Operation::Subscription,
		"GraphqlSubscription only supports subscription operations");

	template<typename... Args>
	GraphqlSubscription(
		WebsocketClient &client, SharedCredentials &creds,
		std::string label,
		std::function<void(caffql::GraphqlResponse<
				   typename OperationField::ResponseData>)>
			messageHandler,
		std::function<void(WebsocketClient::ConnectionEndType)>
			endedHandler,
		Args const &...args)
		: client_(client),
		  creds_(creds),
		  label_(std::move(label)),
		  messageHandler_(std::move(messageHandler)),
		  endedHandler_(std::move(endedHandler))
	{
		auto requestJson = OperationField::request(args...);
		connectionParams_ = Json::object(
			{{"type", "start"}, {"payload", requestJson}});
	}

	~GraphqlSubscription() { disconnect(); }

	void connect()
	{
		disconnect();

		std::weak_ptr<GraphqlSubscription> weakThis =
			this->shared_from_this();

		auto opened = [weakThis](
				      WebsocketClient::Connection connection) {
			if (auto strongThis = weakThis.lock()) {
				Json connectionInit{
					{"type", "connection_init"},
					{"payload",
					 Json::object(
						 {{"X-Credential",
						   strongThis->creds_.lock()
							   .credentials
							   .credential}})}};
				strongThis->client_.sendMessage(
					connection, connectionInit.dump());
				strongThis->client_.sendMessage(
					connection,
					strongThis->connectionParams_.dump());
			}
		};

		auto ended =
			[weakThis](WebsocketClient::Connection connection,
				   WebsocketClient::ConnectionEndType endType) {
				if (auto strongThis = weakThis.lock()) {
					if (strongThis->endedHandler_) {
						strongThis->endedHandler_(
							endType);
					}
				}
			};

		auto messageReceived = [weakThis](WebsocketClient::Connection
							  connection,
						  std::string const &message) {
			Json json;
			try {
				json = Json::parse(message);
			} catch (...) {
				CAFF_LOG_ERROR(
					"Failed to parse graphql subscription message");
				return;
			}

			auto typeIt = json.find("type");
			if (typeIt == json.end() || *typeIt != "data") {
				return;
			}

			caffql::GraphqlResponse<
				typename OperationField::ResponseData>
				response;

			try {
				response = OperationField::response(
					json.at("payload"));
			} catch (std::exception const &error) {
				CAFF_LOG_ERROR(
					"Failed to unpack graphql subscription message: %s",
					error.what());
				return;
			}

			auto strongThis = weakThis.lock();
			if (!strongThis) {
				return;
			}

			if (auto errors = std::get_if<
				    std::vector<caffql::GraphqlError>>(
				    &response)) {
				// Refresh credentials if needed
				bool shouldRefresh = false;
				for (auto const &error : *errors) {
					if (error.message ==
					    "credential expired") {
						shouldRefresh = true;
						break;
					}
				}

				if (shouldRefresh) {
					if (refreshCredentials(
						    strongThis->creds_)) {
						strongThis->connect();
						return;
					}
				}
			}

			if (strongThis->messageHandler_) {
				strongThis->messageHandler_(response);
			}
		};

		connection_ = client_.connect(realtimeGraphqlUrl, label_,
					      opened, ended, messageReceived);
	}

private:
	WebsocketClient &client_;
	SharedCredentials &creds_;
	std::string label_;
	Json connectionParams_;
	std::optional<WebsocketClient::Connection> connection_;
	std::function<void(
		caffql::GraphqlResponse<typename OperationField::ResponseData>)>
		messageHandler_;
	std::function<void(WebsocketClient::ConnectionEndType)> endedHandler_;

	void disconnect()
	{
		if (connection_) {
			client_.close(std::move(*connection_));
			connection_.reset();
		}
	}
};

} // namespace caff