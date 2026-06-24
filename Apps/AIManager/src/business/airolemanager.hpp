#pragma once

#include <vector>

#include "database/airole.hpp"
#include "database/recordmanager.hpp"

class AIRoleManager
{
public:
    AIRoleManager(Database::RecordManager& recordManager);

    void readDatabase();

    bool addRole(const DBRecords::AIRole& role);
    bool updateRole(const DBRecords::AIRole& role);
    void removeRole(const DBRecords::AIRole& role);

private:
    Database::RecordManager& m_recordManager;
    std::vector<DBRecords::AIRole> m_roles;
};
