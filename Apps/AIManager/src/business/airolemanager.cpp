#include "airolemanager.hpp"

#include <Components/Logger/Logger.h>

void AIRoleManager::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_pRecordManager = pManager;
}

void AIRoleManager::readDatabase()
{
    m_roles = m_pRecordManager->getAllRecords<DBRecords::AIRole>();
    for (auto& role : m_roles) {
        COMPLOG_DEBUG("Loaded model role:", role.getId(), role.getVersion(), role.getName(), role.getType(), role.getConfig());
    }
    COMPLOG_DEBUG("Loaded model role total count:", m_roles.size());
}

bool AIRoleManager::addRole(const DBRecords::AIRole &role)
{
    COMPLOG_WARNING("Add role not implemented");
    return false;
}

bool AIRoleManager::updateRole(const DBRecords::AIRole &role)
{
    COMPLOG_WARNING("Update role not implemented");
    return false;
}

void AIRoleManager::removeRole(const DBRecords::AIRole &role)
{
    COMPLOG_WARNING("Remove role not implemented");
}
