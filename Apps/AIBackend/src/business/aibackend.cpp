#include "aibackend.hpp"

#include "websocketeventlistener.hpp"
#include "ollama/ollamainterface.hpp"

#include <ProjectAgency/AIObjects/OllamaConfigMaster.h>
#include <ProjectAgency/Exchange/Error.h>

#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/Utility.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Thread/ProcessInvoker.h>

#include <nlohmann/json.hpp>

struct AIBackend::Impl
{
    WebsocketEventListener  eventListener;
    OllamaInterface         ollamaInterface;

    AIObjects::OllamaConfigMaster             configMaster;
    std::shared_ptr<AIObjects::OllamaConfig>  currentOllamaConfig;

    AIObjects::AIRequest requestBase;
    std::atomic_uint64_t m_currentResponseNo {1};

    std::string createRequestPayload(const OllamaInterface::ResponsePtr& pResp) {
        nlohmann::json res;
        res["id"] = pResp->getId();
        res["pending"] = pResp->isPending();
        res["error_text"] = pResp->getErrorText();
        if (pResp->isPending()) {
            res["data"] = {};
        } else {
            res["data"] = pResp->getResponse().toJson();
        }
        return res.dump();
    }
};

AIBackend::AIBackend() :
    d {new Impl}
{
    initEventProcessing();
    initOllamaInterface();
}

AIBackend::~AIBackend()
{

}

void AIBackend::start(
    const std::string &managerToken,
    uint16_t eventListenPort,
    const std::string &ollamaServerAddress, uint16_t ollamaAPIPort)
{
    d->m_currentResponseNo = 1;

    auto& dirManager = Common::DirectoryManager::getInstance();
    auto configDir = dirManager.getDirectory(Common::DirectoryType::Config);
    auto configFile = configDir / "model.mf";

    d->currentOllamaConfig = d->configMaster.loadConfig(configFile);
    if (!d->currentOllamaConfig) {
        throw Exchange::Error(Exchange::ErrorCode::SystemInvalidConfig, "No model configuration found! Add it in configs dir as a model.mf file");
    }

    d->requestBase.setStream(d->currentOllamaConfig->m_extra.stream);
    d->requestBase.setModel(d->currentOllamaConfig->m_model.name);

    double kaliveD = d->currentOllamaConfig->m_extra.keepAliveS;
    d->requestBase.setKeepAlive(std::to_string(kaliveD / 60.0) + "m");

    d->eventListener.setManagerToken(managerToken);
    d->ollamaInterface.setAPIserver(ollamaServerAddress, ollamaAPIPort);

    COMPLOG_INFO("Starting AI backend...");

    const uint8_t wsThreadCount = 3; // 3 must be enough (1 for answer await, 2 for status / command / etc, 3 is extra)
    d->eventListener.listen(eventListenPort, wsThreadCount);
}

void AIBackend::stop()
{
    d->eventListener.stop();
    d->m_currentResponseNo = 1;
}

void AIBackend::initEventProcessing()
{
    initEventProcessingAIAsk();
}

void AIBackend::initEventProcessingAIAsk()
{
    // AI asking processors
    namespace Events = Exchange::Events;
    d->eventListener.setEventCallback(Events::AIAsk, [this](auto&& wsEvent){
        auto req = d->requestBase;
        req.setRequest(wsEvent.getPayload().data());
        auto pResponse = d->ollamaInterface.ask(std::move(req));

        if (pResponse) {
            wsEvent.setPayload(d->createRequestPayload(pResponse));
        }
        d->eventListener.sendResponse(wsEvent.toJson());
    });
    d->eventListener.setEventCallback(Events::AIAskStatus, [this](auto&& wsEvent){
        auto targetId = wsEvent.getPayload();
        auto pRequest = d->ollamaInterface.getRequest(targetId);
        if (pRequest) {
            nlohmann::json resp;
            resp["pending"] = pRequest->isPending();
            resp["error_text"] = pRequest->getErrorText();
            wsEvent.setPayload(resp.dump());
        } else {
            wsEvent.setPayload("not found");
        }
        d->eventListener.sendResponse(wsEvent.toJson());
    });
    d->eventListener.setEventCallback(Events::AIAskInterrupt, [this](auto&& wsEvent){
        auto res = d->ollamaInterface.interruptAsk(wsEvent.getPayload());
        wsEvent.setPayload(res ? "ok" : "fail");
        d->eventListener.sendResponse(wsEvent.toJson());
    });
    d->eventListener.setEventCallback(Events::AIAskSetConfig, [this](auto&& wsEvent) {
        auto pConfig = AIObjects::OllamaConfigMaster::fromText(wsEvent.getPayload().data());
        auto readSucceed = (pConfig.use_count() != 0);
        if (readSucceed) {
            d->currentOllamaConfig = pConfig;
        }
        wsEvent.setPayload(readSucceed ? "ok" : "fail");
        d->eventListener.sendResponse(wsEvent.toJson());
    });
}

void AIBackend::initOllamaInterface()
{
    namespace Events = Exchange::Events;
    d->ollamaInterface.setAskEventCallback([this](auto&& pResponse) -> void {
        Events::WSEvent resp(Events::AIAsk);
        d->m_currentResponseNo.fetch_add(1, std::memory_order_seq_cst);
        resp.setId(d->m_currentResponseNo.load(std::memory_order_seq_cst));
        resp.setPayload(d->createRequestPayload(pResponse));
        d->eventListener.sendResponse(resp.toJson());
    });
}
