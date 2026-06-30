#include "airolemanager.hpp"

#include <Components/Logger/Logger.h>

AIRoleManager::AIRoleManager(Database::RecordManager& recordManager) :
    m_recordManager {recordManager} {

}

void AIRoleManager::readDatabase()
{
    m_roles = m_recordManager.getAllRecords<DBRecords::AIRole>();
    for (auto& role : m_roles) {
        COMPLOG_DEBUG("Loaded", role.getId(), role.getVersion(), role.getName(), role.getType(), role.getConfig());
    }
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
