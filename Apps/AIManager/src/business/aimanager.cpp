#include "aimanager.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Filework/Common.h>

#include <nlohmann/json.hpp>

#include "common/settings.hpp"

void AIManager::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_pRecordManager = pManager;
}

void AIManager::setToken(const std::string &tokenString)
{
    m_token = tokenString;
}

void AIManager::setInputModel(const std::string &modelName)
{
    m_inputModelName = modelName;
}

void AIManager::init()
{
    auto& appSettings = Common::ApplicationSettings::getInstance();
    auto token = appSettings.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN)->getValueString(); // Expected existance here
    auto backendRecords = m_pRecordManager->getAllRecords<DBRecords::BackendInfo>();
    for (auto& bck : backendRecords) {
        COMPLOG_DEBUG("Loaded backend:", bck.getId(), bck.getDisplayName(), "(", bck.getFullAddress(), ")");
        auto pBackend = std::make_shared<AIBackendHandler>();
        bck.setToken(token);
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

void AIManager::setCurrentTask(const std::string &taskText)
{
    // TODO: Use input model to handle task
    COMPLOG_INFO("Got user task:\n", taskText);
}


