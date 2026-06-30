#pragma once

#include <vector>

#include "database/airole.hpp"
#include "database/recordmanager.hpp"

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
