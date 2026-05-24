#pragma once

#include <drogon/drogon.h>

#include <Components/SystemProcessing/StatusManager.h>

#include "controllerbase.hpp"

#include <ProjectAgency/ExchangeHTTP.h>

class ServerEventProcessor;

class ServerController : public drogon::HttpController<ServerController, false>,
                         public ControllerBase
{
public:
    void setServerEventProcessor(const std::shared_ptr<ServerEventProcessor>& pProcessor);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ServerController::processGetStatus, Exchange::HTTPv1::SERVER_STATUS, drogon::Get);
    METHOD_LIST_END

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    void processGetStatus(const drogon::HttpRequestPtr &req,
                            ResponseCallback_t &&callback);

private:
    SystemProcessing::StatusManager         m_statusManager;
    std::shared_ptr<ServerEventProcessor>   m_serverEventProcessor;
};

