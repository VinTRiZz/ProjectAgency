#include "aibackendhandler.hpp"

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <nlohmann/json.hpp>
#include <thread>

#include <ProjectAgency/AIRequest.h>
#include <ProjectAgency/AIResponse.h>

#include <Components/Logger/Logger.h>

using Client = websocketpp::client<websocketpp::config::asio_client>;
using ConnectionHdl = websocketpp::connection_hdl;
using MessagePtr = websocketpp::config::asio_client::message_type::ptr;

struct AIBackendHandler::Impl
{
    std::string backendAddress;
    std::string token;
    std::string displayName;

    Client                              eventClient;
    websocketpp::lib::asio::io_service  ioService;
    ConnectionHdl                       eventConnection;
    std::atomic<bool>                   connected {false};
};

AIBackendHandler::AIBackendHandler(const std::string &backendAddress) :
    d {new Impl()}
{
    d->backendAddress = backendAddress;

    d->eventClient.init_asio(&d->ioService);

    d->eventClient.set_open_handler([this](ConnectionHdl hdl) {
        d->eventConnection = hdl;
        d->connected.store(true, std::memory_order_release);
        COMPLOG_OK("[AIBackendHandler]", this, "Connected to server");
    });

    d->eventClient.set_message_handler([this](ConnectionHdl hdl, MessagePtr msg) {
        if (msg->get_opcode() == websocketpp::frame::opcode::text) {
            std::string payload = msg->get_payload();
            COMPLOG_DEBUG("[WS] Text got:", payload);
            return;
        }
        COMPLOG_WARNING("[AIBackendHandler]", this, "Skipped binary message");
    });


    d->eventClient.set_close_handler([this](ConnectionHdl hdl) {
        websocketpp::close::status::value code;
        std::string reasonStr;
        auto pCon = d->eventClient.get_con_from_hdl(hdl);
        code = pCon->get_remote_close_code();
        reasonStr = pCon->get_remote_close_reason();
        d->connected.store(false, std::memory_order_release);
        COMPLOG_WARNING("[AIBackendHandler]", this, "Disconnected from:", pCon->get_remote_endpoint(), "reason:", reasonStr, "code:", code);
    });


    d->eventClient.set_fail_handler([this](ConnectionHdl hdl) {
        d->connected.store(false, std::memory_order_release);
        auto pCon = d->eventClient.get_con_from_hdl(hdl);
        auto ec = pCon->get_ec();
        COMPLOG_ERROR("[AIBackendHandler]", this, "Failed to connect to:", pCon->get_remote_endpoint(), "reason:", ec.message());
    });
}

AIBackendHandler::~AIBackendHandler()
{
    disconnect();
}

void AIBackendHandler::setToken(const std::string &token)
{
    d->token = token;
}

void AIBackendHandler::connect()
{
    std::string uri = "ws://" + d->backendAddress + "/?manager=" + d->token;
    COMPLOG_SYNC_DEBUG(uri);
    websocketpp::lib::error_code ec;
    auto con = d->eventClient.get_connection(uri, ec);
    if (ec) {
        COMPLOG_ERROR("[AIBackendHandler]", this, "Connection failed:", ec.message());
        return;
    }
    d->eventClient.connect(con);
    std::thread([this](){
        d->eventClient.run();
    }).detach();
}

void AIBackendHandler::disconnect()
{
    if (!isConnected()) {
        return;
    }
    d->eventClient.close(d->eventConnection, websocketpp::close::status::going_away, "Normal disconnection");
}

bool AIBackendHandler::isConnected() const
{
    return d->connected.load(std::memory_order_acquire);
}

bool AIBackendHandler::sendEvent(const DataObjects::Events::WSEvent &ev)
{
    if (!isConnected()) {
        return false;
    }
    d->eventClient.send(d->eventConnection, ev.toJson(), websocketpp::frame::opcode::text);
    return true;
}

void AIBackendHandler::setDisplayName(const std::string &displayName)
{
    d->displayName = displayName;
}

std::string_view AIBackendHandler::getDisplayName() const
{
    return d->displayName;
}
