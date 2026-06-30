#pragma once

#include <drogon/drogon.h>
#include "controllerbase.hpp"

#include "business/aimanager.hpp"

#include <ProjectAgency/Exchange/HTTP.h>

/**
 * @brief The AIStatusController class Handles status of AIBackend, such as current prompt, system resources, etc.
 */
class AIStatusController : public drogon::HttpController<AIStatusController, false>,
                           public ControllerBase
{
public:
    AIStatusController(AIManager& aiManager);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(
            AIStatusController::processGetStatus,
            Exchange::HTTPv1::BACKEND_STATUS,
            drogon::Get);
    METHOD_LIST_END

    // ID, name, role, current status, last online, etc. (basic info to display)
    void processGetStatus(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback,
        const std::string& backendId);

private:
    AIManager& m_aiManager;
};
