#include "airolemanager.hpp"

#include <Components/Logger/Logger.h>

AIRoleManager::AIRoleManager(Database::RecordManager& recordManager) :
    m_recordManager {recordManager} {

}

void AIRoleManager::readDatabase()
{
    DBRecords::AIRole sampleRecord;
    auto idColName = sampleRecord.getIdColumn().data();
    auto ids = m_recordManager.executeQuery(
        std::string("SELECT ") + idColName +
        " FROM " + sampleRecord.getTable().data() +
        " ORDER BY " + idColName + " ASC");

    m_roles.clear();
    m_roles.reserve(ids.size());
    for (auto& idRecord : ids) {
        auto id = std::get<int64_t>(idRecord[idColName]);
        auto rec = m_recordManager.getRecord<DBRecords::AIRole>(id);
        if (!rec.has_value()) {
            COMPLOG_WARNING("Failed to load role with id:", id);
            continue;
        }
        m_roles.emplace_back(std::move(rec.value()));

        auto& lst = m_roles.back();
        COMPLOG_DEBUG("Loaded", lst.getId(), lst.getVersion(), lst.getName(), lst.getType(), lst.getConfig());
    }
    m_roles.shrink_to_fit();
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
