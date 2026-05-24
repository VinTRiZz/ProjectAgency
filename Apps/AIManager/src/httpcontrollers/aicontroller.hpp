#pragma once

#include <drogon/drogon.h>

#include "business/aimanager.hpp"
#include "controllerbase.hpp"

#include <ProjectAgency/ExchangeHTTP.h>

class AIController : public drogon::HttpController<AIController, false>,
                     public ControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AIController::processGetBackends,         Exchange::HTTPv1::BACKENDS_GET,         drogon::Get);
        ADD_METHOD_TO(AIController::processGetBackendConfig,    Exchange::HTTPv1::BACKEND_CONFIG_GET,   drogon::Get);
        ADD_METHOD_TO(AIController::processSetBackendConfig,    Exchange::HTTPv1::BACKEND_CONFIG_SET,   drogon::Get);
        ADD_METHOD_TO(AIController::processStartDevelop,        Exchange::HTTPv1::DEVELOP_START,        drogon::Get);
        ADD_METHOD_TO(AIController::processGetDevelopStatus,    Exchange::HTTPv1::DEVELOP_STATUS,       drogon::Get);
        ADD_METHOD_TO(AIController::processStopDevelop,         Exchange::HTTPv1::DEVELOP_STOP,         drogon::Get);
    METHOD_LIST_END

    explicit AIController(AIManager& aiMan);

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    // Get list of existing backends with their online status
    void processGetBackends(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback);

    // Backend setting up
    void processGetBackendConfig(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback);
    void processSetBackendConfig(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback);

    // Develop (asking to user's request, if simple)
    void processStartDevelop(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback);
    void processGetDevelopStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback);
    void processStopDevelop(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback);

private:
    AIManager& m_aiManager;
};
