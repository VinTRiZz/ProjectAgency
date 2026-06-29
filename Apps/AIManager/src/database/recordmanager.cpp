#include "recordmanager.hpp"

#include <drogon/drogon.h>

#include <Components/Logger/Logger.h>

namespace Database {

RecordManager::RecordManager(const std::string &appName, const std::string &connectionName) :
    m_connection(appName, connectionName)
{

}

RecordManager::~RecordManager()
{

}

DBConnection &RecordManager::getConnection()
{
    return m_connection;
}

int RecordManager::removeRecord(const std::string& tableName, const std::string& whereCondition)
{
    std::string query = "DELETE FROM " + tableName + (whereCondition.empty() ? "" : std::string(" WHERE ") + whereCondition);

    try {
        m_pClient->execSqlSync(query);
    } catch (const drogon::orm::DrogonDbException& ex) {
        COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
        return false;
    }
    return true;
}

bool RecordManager::addRecord(bool isSync, const std::string &tableName, const std::map<std::string, recordValue_t> &valueMap) const
{
    if (isSync) {
        try {
            m_pClient->execSqlSync(createInsertQuery(tableName, valueMap));
        } catch (const drogon::orm::DrogonDbException& ex) {
            COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
            return false;
        }
    } else {
        m_pClient->execSqlAsync(createInsertQuery(tableName, valueMap), [](const drogon::orm::Result& res){
            // TODO: Process?
        }, [](const drogon::orm::DrogonDbException& ex){
                                    COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
                                });
        return true;
    }
    return false;
}

bool RecordManager::updateRecord(bool isSync, const std::string& tableName, const std::string& whereCondition, const std::map<std::string, recordValue_t>& valueMap)
{
    if (isSync) {
        try {
            m_pClient->execSqlSync(createUpdateQuery(tableName, whereCondition, valueMap));
        } catch (const drogon::orm::DrogonDbException& ex) {
            COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
            return false;
        }
    } else {
        m_pClient->execSqlAsync(createUpdateQuery(tableName, whereCondition, valueMap), [](const drogon::orm::Result& res){
            // TODO: Process?
        }, [](const drogon::orm::DrogonDbException& ex){
                                    COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
                                });
        return true;
    }
    return false;
}

std::map<std::string, recordValue_t> RecordManager::getRecord(bool isSync, const std::string_view &tableName, const std::string_view &whereCondition) const
{
    std::string query = std::string("SELECT * FROM ") + tableName.data() + " WHERE " + whereCondition.data();
    try {
        auto res = m_pClient->execSqlSync(query);
        auto records = resultToRecords(res);

        if (!records.empty()) {
            return records.front();
        }
    } catch (const drogon::orm::DrogonDbException& ex) {
        COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
        return {};
    }
    return {};
}

std::string RecordManager::createInsertQuery(const std::string_view &tableName, const std::map<std::string, recordValue_t> &valueMap) const
{
    std::string colsQuery;
    std::string valuesQuery;
    for (auto& [colName, colValue] : valueMap) {
        colsQuery += colName + ",";
        valuesQuery += cellDataToString(colValue) + ",";
    }
    colsQuery.pop_back();
    valuesQuery.pop_back();

    return std::string("INSERT INTO ") + tableName.data() + " (" + colsQuery + ") VALUES (" + valuesQuery + ")";
}

std::string RecordManager::createUpdateQuery(const std::string_view &tableName, const std::string_view &whereCondition, const std::map<std::string, recordValue_t> &valueMap) const
{
    std::string query("UPDATE ");
    query += tableName.data();
    query += " SET ";

    for (auto& [colName, colValue] : valueMap) {
        query += colName + "=" + cellDataToString(colValue) + ",";
    }
    query.pop_back();
    query += (whereCondition.empty() ? "" : std::string(" WHERE ") + whereCondition.data());

    return query;
}



} // namespace Database
