#pragma once

#include <vector>

#include <ProjectAgency/DB/AIRole.h>
#include <ProjectAgency/DB/RecordManager.h>

class AIRoleManager
{
public:
    void setRecordManager(const Database::RecordManagerPtr& pManager);

    void readDatabase();

    bool addRole(const DBRecords::AIRole& role);
    bool updateRole(const DBRecords::AIRole& role);
    void removeRole(const DBRecords::AIRole& role);

private:
    Database::RecordManagerPtr m_pRecordManager;
    std::vector<DBRecords::AIRole> m_roles;
};
