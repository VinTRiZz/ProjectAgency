#include "airolemanager.hpp"

#include <Components/Logger/Logger.h>

void AIRoleManager::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_pRecordManager = pManager;
}

void AIRoleManager::readDatabase()
{
    auto roleRecords = m_pRecordManager->getAllRecords<DBRecords::AIRole>();
    for (auto& roleRec : roleRecords) {
        auto pRole = std::make_shared<DBRecords::AIRole>(std::move(roleRec));
        m_roles.push_back(pRole);
    }
    COMPLOG_OK("Loaded model role total count:", m_roles.size());
}

bool AIRoleManager::addRole(const DBRecords::AIRolePtr &role)
{
    if (!role || (Exchange::NULL_ID == role->getId())) {
        COMPLOG_WARNING("Invalid AI role passed for add (not inited)");
        return false;
    }

    for (auto pRole : m_roles) {
        if (role->getId() == pRole->getId()) {
            COMPLOG_WARNING("Failed to add AI role (same id exist)");
            return false;
        }
    }

    auto addRes = m_pRecordManager->addRecord(*role);
    if (!addRes.has_value()) {
        m_pRecordManager->getError().printSelf();
        return false;
    }
    role->setId(addRes.value());
    m_roles.push_back(role);
    return true;
}

bool AIRoleManager::updateRole(const DBRecords::AIRolePtr &role)
{
    if (!role || (Exchange::NULL_ID == role->getId())) {
        COMPLOG_WARNING("Invalid AI role passed for update (not inited)");
        return false;
    }
    for (auto pRole : m_roles) {
        if (role->getId() != pRole->getId()) {
            continue;
        }
        if (!m_pRecordManager->updateRecord(*role)) {
            m_pRecordManager->getError().printSelf();
            return false;
        }
        COMPLOG_INFO("AI role with id [", role->getId(), "] configuration updated");
        *pRole = std::move(*role);
        return true;
    }
    return false;
}

void AIRoleManager::removeRole(const DBRecords::AIRole::id_t &id)
{
    if (Exchange::NULL_ID == id) {
        COMPLOG_WARNING("Invalid AI role passed for remove (NULL id)");
        return;
    }
    auto targetIt = std::find_if(m_roles.begin(), m_roles.end(), [&id](auto pRole){
        return (id == pRole->getId());
    });
    if (m_roles.end() == targetIt) {
        COMPLOG_WARNING("AI role with id [", id, "] not found for removing");
        return;
    }
    auto pRole = *targetIt;
    if (!m_pRecordManager->removeRecord(*pRole)) {
        m_pRecordManager->getError().printSelf();
        return;
    }
    m_roles.erase(targetIt);
    COMPLOG_INFO("AI role with id [", id, "] removed");
}
