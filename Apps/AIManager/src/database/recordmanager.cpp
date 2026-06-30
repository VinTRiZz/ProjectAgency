#include "recordmanager.hpp"

#include <drogon/drogon.h>

#include <Components/Logger/Logger.h>

namespace Database {

RecordManager::RecordManager()
{

}

RecordManager::~RecordManager()
{

}

DBConnection &RecordManager::getConnection()
{
    return m_connection;
}

std::string RecordManager::recordToColumns(const record_t &rec) const {
    std::string query;
    for (auto& [colName, colValue] : rec) {
        query += colName + ",";
    }
    if (!query.empty()) {
        query.pop_back(); // last ','
    }
    return query;
}

std::string RecordManager::recordToValues(const record_t &rec) const {
    std::string query;
    for (auto& [colName, colValue] : rec) {
        query += m_connection.cellDataToString(colValue) + ",";
    }
    if (!query.empty()) {
        query.pop_back(); // last ','
    }
    return query;
}

std::string RecordManager::recordToValueAssignList(const record_t &rec) const {
    std::string query;
    for (auto& [colName, colValue] : rec) {
        query += colName + "=" + m_connection.cellDataToString(colValue) + ",";
    }
    if (!query.empty()) {
        query.pop_back(); // last ','
    }
    return query;
}


} // namespace Database
