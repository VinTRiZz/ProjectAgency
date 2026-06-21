#include "userrequestcontroller.hpp"

UserRequestController::UserRequestController(AIManager &aiManager) :
    drogon::HttpController<UserRequestController, false>(),
    ControllerBase(),
    m_aiManager {aiManager} {

}

void UserRequestController::processStartRequest(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    sendTextMessage(drogon::k501NotImplemented, "Can not start task", std::move(callback));
}

void UserRequestController::processGetRequestStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    sendTextMessage(drogon::k501NotImplemented, "Can not get task status", std::move(callback));
}

void UserRequestController::processStopRequest(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    sendTextMessage(drogon::k501NotImplemented, "Can not stop task", std::move(callback));
}
