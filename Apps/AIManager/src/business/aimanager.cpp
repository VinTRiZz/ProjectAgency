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

void AIManager::setToken(const std::string &tokenString)
{
    m_token = tokenString;
}

void AIManager::setPlanningModel(const std::string &modelName)
{
    m_plannerModel = modelName;
}

bool AIManager::init()
{
    // TODO: Load backends info from a database
    COMPLOG_WARNING("Backend info not implemented");
    return false;
}

void AIManager::start()
{
    for (auto& pBackend : m_backends) {
        pBackend->setToken(m_token);
        pBackend->connect();
    }
}

void AIManager::stop()
{
    for (auto& pBackend : m_backends) {
        pBackend->disconnect();
    }
}


