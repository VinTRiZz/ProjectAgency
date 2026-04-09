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
    namespace Events = DataObjects::Events;
    d->eventListener.setEventCallback(Events::AIAsk, [this](auto&& wsEvent){
        auto req = d->requestBase;
        req.setRequest(wsEvent.getPayload().data());

        d->isAnswering.store(true, std::memory_order_release);
        auto response = d->ollamaInterface.askSync(req);
        d->isAnswering.store(false, std::memory_order_release);

        Events::WSEvent resp(Events::AIAsk);
        resp.setPayload(response.getResponse());
        d->eventListener.sendResponse(resp.toJson());
    });
    d->eventListener.setEventCallback(Events::AIStatus, [this](auto&& wsEvent){
        Events::WSEvent resp(Events::AIStatus);
        resp.setPayload(d->isAnswering.load(std::memory_order_acquire) ? "answering" : "idle");
        d->eventListener.sendResponse(resp.toJson());
    });
}
