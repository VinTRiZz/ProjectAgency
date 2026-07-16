#pragma once

#include <drogon/drogon.h>
#include "controllerbase.hpp"

#include "business/aimanager.hpp"

#include <ProjectAgency/Exchange/HTTP.h>

/**
 * @brief The AIServiceController class Handles service of AIBackend instance
 */
class AIServiceController : public drogon::HttpController<AIServiceController, false>,
                            public ControllerBase
{
public:
    AIServiceController(AIManager& aiManager);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(
            AIServiceController::processGetBackendIdList,
            Exchange::HTTPv1::BACKENDS_ID_LIST,
            drogon::Get);

        ADD_METHOD_TO(
            AIServiceController::processAddConfiguration,
            Exchange::HTTPv1::BACKEND_CONFIG_ADD,
            drogon::Post);

        ADD_METHOD_TO(
            AIServiceController::processGetConfiguration,
            Exchange::HTTPv1::BACKEND_CONFIG_GET,
            drogon::Get);

        ADD_METHOD_TO(
            AIServiceController::processSetConfiguration,
            Exchange::HTTPv1::BACKEND_CONFIG_SET,
            drogon::Put);

        ADD_METHOD_TO(
            AIServiceController::processRemoveConfiguration,
            Exchange::HTTPv1::BACKEND_CONFIG_REM,
            drogon::Delete);

        ADD_METHOD_TO(
            AIServiceController::processBackendReconnect,
            Exchange::HTTPv1::BACKEND_RECONNECT,
            drogon::Delete);
    METHOD_LIST_END

    // ID, name, role, current status, last online, etc. (basic info to display)
    void processGetBackendIdList(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback);

    // Configuration of a device
    void processAddConfiguration(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback);

    void processGetConfiguration(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback,
        const std::string& backendId);

    void processSetConfiguration(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback);

    void processRemoveConfiguration(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback,
        const std::string& backendId);

    void processBackendReconnect(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback,
        const std::string& backendId);

private:
    AIManager& m_aiManager;
};
