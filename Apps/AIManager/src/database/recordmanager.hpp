#pragma once

#include <optional>
#include <string>
#include <map>
#include <memory>
#include <vector>

#include "recordobjects.hpp"

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

    void init();

    void setServer(const std::string& address, uint16_t port);
    void setDatabase(const std::string& databaseName);
    void setUser(const std::string& username, const std::string& password);

    template <bool isSync = true, typename T, typename IdT>
    bool addRecord(T&& iValue) {
        return addRecord(isSync, iValue.getTable(), iValue.toRecord());
    }

    template <bool isSync = true, typename T>
    bool updateRecord(T&& iValue, const std::string& whereCondition) {
        return updateRecord(isSync, iValue.getTable(), whereCondition, iValue.toRecord());
    }

    int removeRecord(const std::string &tableName, const std::string &whereCondition);

    template <typename T, bool isSync = true>
    std::optional<T> getRecord(const DataObjects::id_t recordId) const {
        T res;
        auto whereC = std::string(res.getIdColumn().data()) + " = " + std::to_string(recordId);
        if (!res.initFromRecord(getRecord(isSync, res.getTable(), whereC))) {
            return {};
        }
        return res;
    }

    template <typename T, bool isSync = true>
    std::optional<T> getRecord(const std::string& recordStrId) const {
        T res;
        auto whereC = std::string(res.getIdColumn().data()) + " = '" + recordStrId + "'";
        if (!res.initFromRecord(getRecord(isSync, res.getTable(), whereC))) {
            return {};
        }
        return res;
    }

    std::vector<record_t> executeQuery(const std::string& queryStr) const;

private:
    bool addRecord(bool isSync, const std::string& tableName, const std::map<std::string, recordValue_t>& valueMap) const;
    bool updateRecord(bool isSync, const std::string& tableName, const std::string& whereCondition, const std::map<std::string, recordValue_t>& valueMap);
    std::map<std::string, recordValue_t> getRecord(bool isSync, const std::string_view& tableName, const std::string_view& whereCondition) const;

    drogon::orm::DbClientPtr m_pClient;

    // Connection info
    std::string m_appName;
    std::string m_connectionName;
    std::string m_address;
    uint16_t    m_port;
    std::string m_databaseName;
    std::string m_username;
    std::string m_password;

    std::string createConnectionString() const;
    std::string cellDataToString(const recordValue_t& val) const;

    std::string createInsertQuery(const std::string_view &tableName, const std::map<std::string, recordValue_t>& valueMap) const;
    std::string createUpdateQuery(const std::string_view &tableName, const std::string_view& whereCondition, const std::map<std::string, recordValue_t>& valueMap) const;
};

} // namespace Database
