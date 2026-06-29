#pragma once

#include <optional>
#include <string>
#include <map>
#include <memory>

#include "recordobjects.hpp"
#include "dbconnection.hpp"

namespace drogon::orm {
class DbClient;
using DbClientPtr = std::shared_ptr<DbClient>;
}

namespace Database {

class RecordManager;
using RecordManagerPtr = std::shared_ptr<RecordManager>;

class RecordManager
{
public:
    explicit RecordManager(const std::string& appName, const std::string& connectionName);
    ~RecordManager();

    DBConnection& getConnection();

    template <typename IdT, bool isSync = true>
    bool addRecord(RecordBase<IdT>&& iValue) {
        return addRecord(isSync, iValue.getTable(), iValue.toRecord());
    }

    template <typename IdT, bool isSync = true>
    bool updateRecord(RecordBase<IdT>&& iValue, const std::string& whereCondition) {
        return updateRecord(isSync, iValue.getTable(), whereCondition, iValue.toRecord());
    }

    template <typename IdT, bool isSync = true>
    std::optional<RecordBase<IdT> > getRecord(const DataObjects::id_t recordId) const {
        RecordBase<IdT> res;
        auto whereC = std::string(res.getIdColumn().data()) + " = " + std::to_string(recordId);
        if (!res.initFromRecord(getRecord(isSync, res.getTable(), whereC))) {
            return {};
        }
        return res;
    }

private:
    // Connection info
    DBConnection m_connection;
};

} // namespace Database
