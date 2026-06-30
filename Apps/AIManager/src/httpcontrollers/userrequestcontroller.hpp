#pragma once

#include <drogon/drogon.h>

#include "business/aimanager.hpp"
#include "controllerbase.hpp"

#include <ProjectAgency/Exchange/HTTP.h>

/**
 * @brief The UserRequestController class Handles user requests (user tasks)
 */
class UserRequestController : public drogon::HttpController<UserRequestController, false>,
                              public ControllerBase
{
public:
    UserRequestController(AIManager& aiManager);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(
            UserRequestController::processStartRequest,
            Exchange::HTTPv1::USER_REQUEST_START,
            drogon::Put);

        ADD_METHOD_TO(
            UserRequestController::processGetRequestStatus,
            Exchange::HTTPv1::USER_REQUEST_STATUS,
            drogon::Get);

        ADD_METHOD_TO(
            UserRequestController::processStopRequest,
            Exchange::HTTPv1::USER_REQUEST_STOP,
            drogon::Put);
    METHOD_LIST_END


    void processStartRequest(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback);

    void processGetRequestStatus(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback);

    void processStopRequest(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback);

private:
    AIManager& m_aiManager;
};
