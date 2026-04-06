#include "servercontroller.hpp"

#include <Components/Logger/Logger.h>

#include <nlohmann/json.hpp>

void ServerController::setServerEventProcessor(const std::shared_ptr<ServerEventProcessor> &pProcessor)
{
    m_serverEventProcessor = pProcessor;
}

void ServerController::processGetStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    sendTextMessage(drogon::k501NotImplemented, "Status not implemented", std::move(callback));
}