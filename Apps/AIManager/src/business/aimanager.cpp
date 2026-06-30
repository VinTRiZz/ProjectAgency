#include "aimanager.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Filework/Common.h>

#include <nlohmann/json.hpp>

AIManager::AIManager()
{

}

AIManager::~AIManager()
{
    stop();
}

void AIManager::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_pRecordManager = pManager;
}

void AIManager::setToken(const std::string &tokenString)
{
    m_token = tokenString;
}

void AIManager::setPlanningModel(const std::string &modelName)
{
    m_plannerModel = modelName;
}

void AIManager::init()
{
    auto backendRecords = m_pRecordManager->getAllRecords<DBRecords::BackendInfo>();
    for (auto& bck : backendRecords) {
        COMPLOG_DEBUG("Loaded backend:", bck.getId(), bck.getDisplayName(), "(", bck.getFullAddress(), ")");
        auto pBackend = std::make_shared<AIBackendHandler>();
        pBackend->getInfo() = bck;
        m_backends.push_back(pBackend);
    }
    COMPLOG_DEBUG("Loaded backend total count:", backendRecords.size());
}

void AIManager::start()
{
    for (auto& pBackend : m_backends) {
        pBackend->connect();
    }
}

void AIManager::stop()
{
    for (auto& pBackend : m_backends) {
        pBackend->disconnect();
    }
}


