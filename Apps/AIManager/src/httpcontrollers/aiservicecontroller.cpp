#include "aiservicecontroller.hpp"

AIServiceController::AIServiceController(AIManager &aiManager) :
    drogon::HttpController<AIServiceController, false>(),
    ControllerBase(),
    m_aiManager {aiManager}
{

}

void AIServiceController::processGetCommonInfo(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    sendTextMessage(drogon::k501NotImplemented, "Can not get common info", std::move(callback));
}

void AIServiceController::processGetConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &backendId)
{
    sendTextMessage(drogon::k501NotImplemented, "Can not get configuration", std::move(callback));
}

void AIServiceController::processSetConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &backendId)
{
    sendTextMessage(drogon::k501NotImplemented, "Can not set config", std::move(callback));
}
