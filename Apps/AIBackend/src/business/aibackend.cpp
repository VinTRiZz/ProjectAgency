#include "aibackend.hpp"

#include "websocketeventlistener.hpp"
#include "ollama/ollamainterface.hpp"

#include <ProjectAgency/OllamaConfigMaster.h>

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

#include <atomic>

struct AIBackend::Impl
{
    WebsocketEventListener  eventListener;
    OllamaInterface         ollamaInterface;

    DataObjects::OllamaConfigMaster             configMaster;
    std::shared_ptr<DataObjects::OllamaConfig>  currentOllamaConfig;

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

    d->eventListener.setManagerToken(managerToken);
    d->ollamaInterface.setAPIserver(ollamaServerAddress, ollamaAPIPort);

    COMPLOG_INFO("Starting AI backend...");
    d->eventListener.listen(eventListenPort);
}

void AIBackend::stop()
{
    d->eventListener.stop();
}

void AIBackend::initEventProcessing()
{
    namespace Events = DataObjects::Events;
    d->eventListener.setEventCallback(Events::AIAsk, [this](auto&& wsEvent){
        DataObjects::AIRequest req;
        req.setModel(d->currentOllamaConfig->model());
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
