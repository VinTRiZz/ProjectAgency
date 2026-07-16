#include "aimanager.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Filework/Common.h>

#include <ProjectAgency/DB/TEST/BackendInfoGenerator.h>

#include <nlohmann/json.hpp>

#include "common/settings.hpp"

void AIManager::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_pRecordManager = pManager;
}

Database::RecordManagerPtr AIManager::getRecordManager() const
{
    return m_pRecordManager;
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
    m_token = appSettings.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN)->getValueString(); // Expected existance here
    auto backendRecords = m_pRecordManager->getAllRecords<DBRecords::AIBackendInfo>();
    for (auto& bckRec : backendRecords) {
        auto bck = std::make_shared<DBRecords::AIBackendInfo>(std::move(bckRec));
        auto pBackend = std::make_shared<AIBackendHandler>();
        bck->setToken(m_token);
        pBackend->setInfo(bck);
        m_backends.push_back(pBackend);

        // Debug needs
        // COMPLOG_DEBUG(
        //     "Loaded:",
        //     bck->getFullAddress(), " | ",
        //     bck->getDisplayName(), " | ",
        //     static_cast<int>(bck->getType()), " | ",
        //     bck->getId());
    }
    COMPLOG_OK("Loaded backend total count:", backendRecords.size());
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
    COMPLOG_INFO("Got user task:\n", taskText);
    m_currentTask = taskText;

    // TODO: Use input model to handle task
    m_error = Exchange::Error(Exchange::ErrorCode::SystemNotImplemented, "setCurrentTask");
    m_error.printSelf();
}

std::string AIManager::getCurrentTask() const
{
    return m_currentTask;
}

bool AIManager::isSolvingTask() const
{
    return !m_currentTask.empty();
}

void AIManager::stopCurrentTask()
{
    // TODO: Stop task processing
    m_currentTask = {};
}

bool AIManager::addBackend(const DBRecords::AIBackendInfoPtr &backendInfo)
{
    m_error.reset();
    if (!backendInfo) {
        m_error.setCode(Exchange::ErrorCode::SystemObjectNotInited);
        m_error.setDetailText("Invalid backend to add");
        return false;
    }

    for (auto pBck : m_backends) {
        if (backendInfo->getId() == pBck->getInfo()->getId()) {
            COMPLOG_WARNING("Failed to add backend (same id exist)");
            return false;
        }
    }

    auto backendId = m_pRecordManager->addRecord(*backendInfo);
    if (!backendId.has_value()) {
        m_error = m_pRecordManager->getError();
        return false;
    }
    backendInfo->setId(backendId.value());

    auto pBackend = std::make_shared<AIBackendHandler>();
    backendInfo->setToken(m_token);
    pBackend->setInfo(backendInfo);
    m_backends.push_back(pBackend);

    COMPLOG_INFO_SYNC("Added backend configuration. Trying to connect...");

    pBackend->connect();
    return true;
}

bool AIManager::updateBackend(const DBRecords::AIBackendInfoPtr &backendInfo)
{
    m_error.reset();
    if (!backendInfo || backendInfo->getId()) {
        m_error.setCode(Exchange::ErrorCode::SystemObjectNotInited);
        m_error.setDetailText("Invalid backend to update");
        return false;
    }
    for (auto pBck : m_backends) {
        if (backendInfo->getId() != pBck->getInfo()->getId()) {
            continue;
        }
        if (!m_pRecordManager->updateRecord(*backendInfo)) {
            m_error = m_pRecordManager->getError();
            return false;
        }
        pBck->setInfo(backendInfo);
        COMPLOG_INFO("Backend with id [", backendInfo->getId().value_or("NULL"), "] configuration updated");
        return true;
    }
    COMPLOG_WARNING("Backend with id [", backendInfo->getId().value_or("NULL"), "] not found for configuration update");
    return false;
}

std::shared_ptr<AIBackendHandler> AIManager::getBackend(const DBRecords::AIBackendInfo::id_nullable_t& backendId) const
{
    for (auto pBck : m_backends) {
        if (backendId == pBck->getInfo()->getId()) {
            return pBck;
        }
    }
    return {};
}

std::vector<std::shared_ptr<AIBackendHandler> > AIManager::getBackends() const
{
    return m_backends;
}

void AIManager::removeBackend(const DBRecords::AIBackendInfo::id_nullable_t &backendId)
{
    m_error.reset();
    if (!backendId) {
        m_error.setCode(Exchange::ErrorCode::SystemObjectNotInited);
        m_error.setDetailText("Invalid backend to remove");
        return;
    }
    auto targetIt = std::find_if(m_backends.begin(), m_backends.end(), [&backendId](auto pBackend){
        return (backendId == pBackend->getInfo()->getId());
    });
    if (m_backends.end() == targetIt) {
        COMPLOG_WARNING("Backend with id [", backendId.value_or("NULL"), "] not found for removing");
        return;
    }
    auto pBackend = *targetIt;
    if (!m_pRecordManager->removeRecord(*pBackend->getInfo())) {
        m_error = m_pRecordManager->getError();
        return;
    }
    m_backends.erase(targetIt);
    COMPLOG_INFO("Backend with id [", backendId.value_or("NULL"), "] removed");
}


