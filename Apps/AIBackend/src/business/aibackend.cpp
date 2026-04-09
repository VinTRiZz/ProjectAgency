#include "aibackend.hpp"

#include "websocketeventlistener.hpp"
#include "ollama/ollamainterface.hpp"

#include <ProjectAgency/OllamaConfigMaster.h>

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Thread/ProcessInvoker.h>

#include <atomic>

struct AIBackend::Impl
{
    WebsocketEventListener  eventListener;
    OllamaInterface         ollamaInterface;

    DataObjects::OllamaConfigMaster             configMaster;
    std::shared_ptr<DataObjects::OllamaConfig>  currentOllamaConfig;

    DataObjects::AIRequest requestBase;

    std::atomic<bool> isAnswering {false};
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
    auto& dirManager = Common::DirectoryManager::getInstance();
    auto configDir = dirManager.getDirectory(Common::DirectoryManager::DirectoryType::Config);
    auto configFile = configDir / "model.mf";

    d->currentOllamaConfig = d->configMaster.loadConfig(configFile);
    if (!d->currentOllamaConfig) {
        throw std::runtime_error("No model configuration found! Add it in configs dir as a model.mf file");
    }

    d->requestBase.setStream(false); // TODO: Enable later (after MVP)
    d->requestBase.setModel(d->currentOllamaConfig->model());
    d->requestBase.setKeepAlive("30m"); // TODO: Discuss

    d->eventListener.setManagerToken(managerToken);
    d->ollamaInterface.setAPIserver(ollamaServerAddress, ollamaAPIPort);

    COMPLOG_INFO("Starting AI backend...");

    const uint8_t wsThreadCount = 3; // 3 must be enough (1 for answer await, 2 for status / command / etc, 3 is extra)
    d->eventListener.listen(eventListenPort, wsThreadCount);
}

void AIBackend::stop()
{
    d->eventListener.stop();
}

void AIBackend::initEventProcessing()
{
    initEventProcessingAIAsk();
}

void AIBackend::initEventProcessingAIAsk()
{
    // AI asking processors
    namespace Events = DataObjects::Events;
    d->eventListener.setEventCallback(Events::AIAsk, [this](auto&& wsEvent){
        auto req = d->requestBase;
        req.setRequest(wsEvent.getPayload().data());

        d->isAnswering.store(true, std::memory_order_release);
        d->ollamaInterface.ask(req);
    });
    d->eventListener.setEventCallback(Events::AIAskStatus, [this](auto&& wsEvent){
        wsEvent.setPayload(d->isAnswering.load(std::memory_order_acquire) ? "busy" : "idle");
        d->eventListener.sendResponse(wsEvent.toJson());
    });
    d->eventListener.setEventCallback(Events::AIAskInterrupt, [this](auto&& wsEvent){
        d->ollamaInterface.askInterrupt();
        wsEvent.setPayload({});
        d->eventListener.sendResponse(wsEvent.toJson());
    });
    d->eventListener.setEventCallback(Events::AIAskSetConfig, [this](auto&& wsEvent) {
        auto pConfig = DataObjects::OllamaConfigMaster::fromText(wsEvent.getPayload().data());
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
    namespace Events = DataObjects::Events;
    d->ollamaInterface.setResponseCallback([this](auto&& response) -> void {
        d->isAnswering.store(false, std::memory_order_release);
        Events::WSEvent resp(Events::AIAsk);
        if (response.has_value()) {
            resp.setPayload(response->getResponse());
        }
        d->eventListener.sendResponse(resp.toJson());
    });
}
