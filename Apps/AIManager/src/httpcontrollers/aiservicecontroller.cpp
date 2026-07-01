#include "aiservicecontroller.hpp"

#include <nlohmann/json.hpp>

AIServiceController::AIServiceController(AIManager &aiManager) :
    drogon::HttpController<AIServiceController, false>(),
    ControllerBase(),
    m_aiManager {aiManager}
{

}

void AIServiceController::processGetBackendIdList(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    nlohmann::json res;
    for (auto& pBck : m_aiManager.getBackends()) {
        res.push_back(pBck->getInfo()->getId());
    }
    sendJsonMessage(drogon::k200OK, res.dump(), std::move(callback));
}

void AIServiceController::processAddConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    DBRecords::BackendInfoPtr bckInfo = std::make_shared<DBRecords::BackendInfo>();
    if (!bckInfo->readJson(req->getBody().data())) {
        sendTextMessage(drogon::k400BadRequest, "Invalid backend info", std::move(callback));
        return;
    }
    if (m_aiManager.addBackend(bckInfo)) {
        sendTextMessage(drogon::k200OK, bckInfo->toJson(), std::move(callback));
        return;
    }
    sendTextMessage(drogon::k400BadRequest, "Backend exist or failed to save", std::move(callback));
}

void AIServiceController::processGetConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &backendId)
{
    for (auto pBck : m_aiManager.getBackends()) {
        if (backendId == pBck->getInfo()->getId()) {
            sendJsonMessage(drogon::k200OK, pBck->getInfo()->toJson(), std::move(callback));
        }
    }
    sendTextMessage(drogon::k404NotFound, "No such backend found", std::move(callback));
}

void AIServiceController::processSetConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    DBRecords::BackendInfoPtr bckInfo = std::make_shared<DBRecords::BackendInfo>();
    if (!bckInfo->readJson(req->getBody().data())) {
        sendTextMessage(drogon::k400BadRequest, "Invalid backend info", std::move(callback));
        return;
    }
    if (m_aiManager.updateBackend(bckInfo)) {
        sendTextMessage(drogon::k200OK, "Configuration changed", std::move(callback));
        return;
    }
    sendTextMessage(drogon::k404NotFound, "No such backend or failed to update", std::move(callback));
}

void AIServiceController::processRemoveConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &backendId)
{
    m_aiManager.removeBackend(backendId);
    sendJsonMessage(drogon::k200OK, "Backend removed", std::move(callback));
}
