#pragma once

#include <vector>

#include <ProjectAgency/DB/AIRole.h>
#include <ProjectAgency/DB/RecordManager.h>

class AIRoleManager
{
public:
    void setRecordManager(const Database::RecordManagerPtr& pManager);

    void readDatabase();

    bool addRole(const DBRecords::AIRolePtr& role);
    bool updateRole(const DBRecords::AIRolePtr& role);
    void removeRole(const DBRecords::AIRole::id_t& id);

private:
    Database::RecordManagerPtr m_pRecordManager;
    std::vector<DBRecords::AIRolePtr> m_roles;
};
