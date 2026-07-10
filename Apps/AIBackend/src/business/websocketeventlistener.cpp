#include "websocketeventlistener.hpp"

#include <Components/Logger/Logger.h>

#include <ProjectAgency/Exchange/Error.h>

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <nlohmann/json.hpp>

#include <map>
#include <regex>

using Server = websocketpp::server<websocketpp::config::asio>;
using ConnectionHdl = websocketpp::connection_hdl;
using MessagePtr = websocketpp::config::asio::message_type::ptr;

struct WebsocketEventListener::Impl
{
    websocketpp::lib::asio::io_service ioService;

    Server deviceEventServer;
    ConnectionHdl managerConnection;

    std::atomic<bool> isListening {false};

    // Connection required
    std::string token;
    std::map<Exchange::Events::EventType, std::function<void(Exchange::Events::WSEvent&&)> > eventCallbacks;

    void stop() {
        if (!deviceEventServer.is_listening()) {
            return;
        }

        Exchange::Error err;

        websocketpp::lib::error_code ec;
        deviceEventServer.stop_listening(ec);
        if (ec) {
            err.setCode(Exchange::ErrorCode::InterfaceStopFailed);
            err.setDetailText(std::string("[WS] ") + ec.message());
            err.printSelfStd();
        }

        try {
            deviceEventServer.close(managerConnection, websocketpp::close::status::going_away, "Server shutdown");
        } catch (const std::exception& ex) {
            Exchange::Error::printSelf(
                Exchange::ErrorCode::InterfaceStopFailed,
                std::string("[WS] ") + ex.what());
        } catch (...) {
            Exchange::Error::printSelf(
                Exchange::ErrorCode::InterfaceStopFailed,
                std::string("[WS] ") + ec.message());
        }

        deviceEventServer.stop();
    }

    ~Impl() {
        stop();
    }
};

WebsocketEventListener::WebsocketEventListener() :
    d {new Impl}
{
    initClient();
    initConnectionProcessing();
    initMessageProcessing();
}

WebsocketEventListener::~WebsocketEventListener()
{

}

void WebsocketEventListener::setManagerToken(const std::string &tokenString)
{
    d->token = tokenString;
}

bool WebsocketEventListener::listen(uint16_t port, uint8_t threadCount)
{
    d->deviceEventServer.listen(port);
    d->deviceEventServer.start_accept();
    for (uint8_t thNo = 1; thNo < threadCount; ++thNo) {
        std::thread([this](){
            d->deviceEventServer.run();
        }).detach();
    }
    d->deviceEventServer.run();
    return true;
}

void WebsocketEventListener::stop()
{
    d->stop();
}

void WebsocketEventListener::setEventCallback(Exchange::Events::EventType evType, std::function<void (Exchange::Events::WSEvent &&)> &&eventCallback)
{
    d->eventCallbacks[evType] = std::move(eventCallback);
}

void WebsocketEventListener::sendResponse(const std::string_view &respText)
{
    if (d->managerConnection.expired()) {
        COMPLOG_WARNING("[WS] Failed to send response (size:", respText.size(), "bytes)");
        return;
    }
    d->deviceEventServer.send(d->managerConnection, respText.data(), websocketpp::frame::opcode::text);
}

void WebsocketEventListener::initClient()
{
    d->deviceEventServer.init_asio(&d->ioService);

    // Common settings
    d->deviceEventServer.set_reuse_addr(true);
    d->deviceEventServer.set_http_handler([this](ConnectionHdl hdl) {
        auto con = d->deviceEventServer.get_con_from_hdl(hdl);
        con->set_body("Invalid protocol (WebSockets required)");
        con->set_status(websocketpp::http::status_code::not_found);
    });
}

void WebsocketEventListener::initConnectionProcessing()
{
    d->deviceEventServer.set_validate_handler([this](ConnectionHdl hdl) -> bool {
        auto con = d->deviceEventServer.get_con_from_hdl(hdl);
        auto remote = con->get_remote_endpoint();

        auto parameters = con->get_resource();
        std::regex devnameRegexp("\\/[?&]manager=([a-f0-9]{64})");
        std::smatch devnameMatch;
        if (!std::regex_match(parameters, devnameMatch, devnameRegexp)) {
            Exchange::Error::printSelf(
                Exchange::ErrorCode::InterfaceConnectionError,
                std::string("[WS] (incoming rejected) invalid target - ") + remote);
            return false;
        }

        auto managerToken = devnameMatch[1].str();
        if (managerToken != d->token) {
            Exchange::Error::printSelf(
                Exchange::ErrorCode::InterfaceConnectionError,
                std::string("[WS] (incoming rejected) invalid token - ") + remote);
            return false;
        }

        if (!d->managerConnection.expired()) {
            Exchange::Error::printSelf(
                Exchange::ErrorCode::InterfaceConnectionError,
                std::string("[WS] (incoming rejected) already authorized - ") + remote);
            return false;
        }

        COMPLOG_OK("[WS] Manager connected:", remote);
        d->managerConnection = con;
        return true;
    });

    d->deviceEventServer.set_close_handler([this](ConnectionHdl hdl) {
        auto con = d->deviceEventServer.get_con_from_hdl(hdl);
        auto code = con->get_remote_close_code();
        auto reasonStr = con->get_remote_close_reason();

        switch (code)
        {
        case websocketpp::close::status::normal:
            COMPLOG_WARNING("[WS] Client", con->get_remote_endpoint(), "disconnected:", reasonStr, "(normal)");
            break;

        case websocketpp::close::status::going_away:
            COMPLOG_WARNING("[WS] Client", con->get_remote_endpoint(), "disconnected:", reasonStr, "(server shutdown)");
            break;

        case websocketpp::close::status::protocol_error:
            COMPLOG_WARNING("[WS] Client", con->get_remote_endpoint(), "disconnected:", reasonStr, "(WS protocol error)");
            break;

        default:
            COMPLOG_WARNING("[WS] Client", con->get_remote_endpoint(), "disconnected:", reasonStr, "code:", code);
        };
    });

    d->deviceEventServer.set_fail_handler([this](ConnectionHdl hdl) {
        auto con = d->deviceEventServer.get_con_from_hdl(hdl);
        auto ec = con->get_ec();
        Exchange::Error::printSelf(
            Exchange::ErrorCode::InterfaceConnectionError,
            std::string("[WS] ") + ec.message());
    });
}

void WebsocketEventListener::initMessageProcessing()
{
    // Message processing
    d->deviceEventServer.set_message_handler([this](ConnectionHdl hdl, MessagePtr msg) {
        if (msg->get_opcode() != websocketpp::frame::opcode::text) {
            COMPLOG_WARNING("[WS] Skipped binary data");
            return;
        }

        Exchange::Events::WSEvent ev;
        if (!ev.readJson(msg->get_payload())) {
            ev.getError().printSelfStd();
            return;
        }

        auto processor = d->eventCallbacks.find(ev.getType());
        if (processor == d->eventCallbacks.end()) {
            COMPLOG_WARNING("[WS] Skipped event of type (no processor found):", (int)ev.getType());
            return;
        }
        processor->second(std::move(ev));
    });
}
