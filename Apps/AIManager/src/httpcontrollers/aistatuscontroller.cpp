#include "aistatuscontroller.hpp"

AIStatusController::AIStatusController(AIManager& aiManager) :
    drogon::HttpController<AIStatusController, false>(),
    ControllerBase(),
    m_aiManager {aiManager}
{

}

void AIStatusController::processGetStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &backendId)
{
    sendTextMessage(drogon::k501NotImplemented, "Can not get status of a backend", std::move(callback));
}
