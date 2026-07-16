#pragma once

#include "controllerbase.hpp"

#include <ProjectAgency/Exchange/HTTP.h>

#include "business/applicationcore.hpp"

class AIManagerServiceController : public drogon::HttpController<AIManagerServiceController, false>,
                                   public ControllerBase
{
public:
    AIManagerServiceController(ApplicationCore& appCore);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(
            AIManagerServiceController::processServerAction,
            Exchange::HTTPv1::SERVER_ACTION,
            drogon::Put);
    METHOD_LIST_END

    // Completely stop or restarts application
    void processServerAction(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback,
        int actionType);

private:
    ApplicationCore& m_appCore;
};
