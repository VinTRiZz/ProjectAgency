#include "aimanager.hpp"

#include "backendhandle/backendhandlerbuilder.hpp"
#include "backendhandle/aibackendhandler.hpp"

#include "httpcontrollers/aicontroller.hpp"
#include "httpcontrollers/servercontroller.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Filework/Common.h>

#include <vector>
#include <fstream>

#include <nlohmann/json.hpp>

#include <drogon/drogon.h>

struct AIManager::Impl
{
    std::string token;
    std::string planningModel;

    BackendHandlerBuilder backendBuilder;
    std::vector<std::shared_ptr<AIBackendHandler> > backends;
};

AIManager::AIManager() :
    d {new Impl}
{

}

AIManager::~AIManager()
{
    stop();
}

void AIManager::initBackends()
{
    auto& dirManager = Common::DirectoryManager::getInstance();
    auto backendInfoFile = dirManager.getDirectory(Common::Config) / "backends.json";

    if (!std::filesystem::exists(backendInfoFile)) {
        std::fstream ofile(backendInfoFile, std::ios_base::out);
    }

    std::string configString;
    if (!Filework::Common::readFileData(backendInfoFile, configString)) {
        COMPLOG_WARNING("[AIManager] Failed to retrieve configuration of AI backends from file");
        return;
    }

    try {
        auto configJson = nlohmann::json::parse(configString);

        if (!configJson.is_array()) {
            COMPLOG_WARNING("[AIManager] Invalid config format (expected array)");
            return;
        }

        for (auto& conf : configJson) {
            auto pBackend = d->backendBuilder.fromConfig(conf);
            if (pBackend) {
                d->backends.push_back(pBackend);
            } else {
                COMPLOG_WARNING("[AIManager] Configuration skipped (failed to proceed):", conf);
            }
        }

    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_WARNING("[AIManager] Failed to read configuration of AI backends:", ex.what());
    }
}

void AIManager::setToken(const std::string &tokenString)
{
    d->token = tokenString;
}

void AIManager::setPlanningModel(const std::string &modelName)
{
    d->planningModel = modelName;
}

void AIManager::start(uint16_t apiPort)
{
    for (auto& pBackend : d->backends) {
        pBackend->setToken(d->token);
        pBackend->connect();
    }

    // Controller setup
    drogon::app().registerController(std::make_shared<ServerController>());
    drogon::app().registerController(std::make_shared<AIController>(*this));

    // Server info
    drogon::app().setServerHeaderField("AIManager");

    // 3 threads, one for pending operations, second for periodic requests, third is extra
    drogon::app().setThreadNum(3);

    drogon::app().addListener("0.0.0.0", apiPort);
    drogon::app().run();
}

void AIManager::stop()
{
    if (!drogon::app().isRunning()) {
        return;
    }

    for (auto& pBackend : d->backends) {
        pBackend->disconnect();
    }
    drogon::app().quit();
}

std::vector<DataObjects::BackendDisplayInfo> AIManager::getBackends() const
{
    std::vector<DataObjects::BackendDisplayInfo> res;
    for (auto& hdl : d->backends) {
        DataObjects::BackendDisplayInfo info;
        info.isOnline = hdl->isConnected();
        info.name = hdl->getDisplayName();

        res.push_back(info);
    }
    return res;
}


