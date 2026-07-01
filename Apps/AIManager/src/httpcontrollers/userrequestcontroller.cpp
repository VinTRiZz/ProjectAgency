#include "userrequestcontroller.hpp"

UserRequestController::UserRequestController(AIManager &aiManager) :
    drogon::HttpController<UserRequestController, false>(),
    ControllerBase(),
    m_aiManager {aiManager} {

}

void UserRequestController::processStartRequest(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    m_aiManager.setCurrentTask(req->getBody().data());
    sendTextMessage(drogon::k200OK, "Task started", std::move(callback));
}

void UserRequestController::processGetRequestStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    if (m_aiManager.isSolvingTask()) {
        sendTextMessage(drogon::k200OK, m_aiManager.getCurrentTask(), std::move(callback));
        return;
    }
    sendTextMessage(drogon::k200OK, {}, std::move(callback));
}

void UserRequestController::processStopRequest(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    m_aiManager.stopCurrentTask();
    if (m_aiManager.isSolvingTask()) {
        sendTextMessage(drogon::k200OK, "Task stopped", std::move(callback));
        return;
    }
    sendTextMessage(drogon::k500InternalServerError, "Failed to stop task solving", std::move(callback));
}
