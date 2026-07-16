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
        res.push_back(pBck->getInfo()->getId().has_value() ?
                            nlohmann::json::value_type(pBck->getInfo()->getId().value()) :
                            nlohmann::json::value_type());
    }
    sendJsonMessage(drogon::k200OK, res.dump(), std::move(callback));
}

void AIServiceController::processAddConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    DBRecords::AIBackendInfo bckInfo;
    if (!bckInfo.readJson(req->getBody().data())) {
        sendTextMessage(drogon::k400BadRequest, bckInfo.getError().what(), std::move(callback));
        return;
    }
    if (m_aiManager.addBackend(bckInfo.toPointer())) {
        sendTextMessage(drogon::k200OK, bckInfo.toJson(), std::move(callback));
        return;
    }
    sendTextMessage(drogon::k500InternalServerError, m_aiManager.getError().what(), std::move(callback));
}

void AIServiceController::processGetConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &backendId)
{
    auto pBackend = m_aiManager.getBackend(backendId);
    if (pBackend) {
        sendJsonMessage(drogon::k200OK, pBackend->getInfo()->toJson(), std::move(callback));
        return;
    }
    sendTextMessage(drogon::k404NotFound, "No such backend", std::move(callback));
}

void AIServiceController::processSetConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    DBRecords::AIBackendInfoPtr bckInfo = std::make_shared<DBRecords::AIBackendInfo>();
    if (!bckInfo->readJson(req->getBody().data())) {
        sendTextMessage(drogon::k400BadRequest, bckInfo->getError().what(), std::move(callback));
        return;
    }
    if (m_aiManager.updateBackend(bckInfo)) {
        sendTextMessage(drogon::k200OK, bckInfo->toJson(), std::move(callback));
        return;
    }
    sendTextMessage(drogon::k500InternalServerError, m_aiManager.getError().what(), std::move(callback));
}

void AIServiceController::processRemoveConfiguration(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &backendId)
{
    auto pBackend = m_aiManager.getBackend(backendId);
    if (!pBackend) {
        sendTextMessage(drogon::k404NotFound, "No such backend", std::move(callback));
        return;
    }
    if (m_aiManager.removeBackend(backendId)) {
        sendTextMessage(drogon::k200OK, backendId, std::move(callback));
        return;
    }
    sendTextMessage(drogon::k500InternalServerError, m_aiManager.getError().what(), std::move(callback));
}

void AIServiceController::processBackendReconnect(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &backendId)
{
    auto pBackend = m_aiManager.getBackend(backendId);
    if (!pBackend) {
        sendTextMessage(drogon::k404NotFound, "No such backend", std::move(callback));
        return;
    }
    pBackend->connect();
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (pBackend->isConnected()) {
        sendTextMessage(drogon::k200OK, {}, std::move(callback));
    } else {
        sendTextMessage(drogon::k503ServiceUnavailable, pBackend->getError().what(), std::move(callback));
    }
}
