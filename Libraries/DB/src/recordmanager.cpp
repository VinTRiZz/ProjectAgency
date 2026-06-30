#include "recordmanager.hpp"

#include <Components/Logger/Logger.h>

namespace Database {

void RecordManager::setConnection(const AbstractConnectionPtr &pCon)
{
    m_connection = pCon;
}

AbstractConnectionPtr RecordManager::getConnection() const
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
        query += m_connection->cellDataToString(colValue) + ",";
    }
    if (!query.empty()) {
        query.pop_back(); // last ','
    }
    return query;
}

std::string RecordManager::recordToValueAssignList(const record_t &rec) const {
    std::string query;
    for (auto& [colName, colValue] : rec) {
        query += colName + "=" + m_connection->cellDataToString(colValue) + ",";
    }
    if (!query.empty()) {
        query.pop_back(); // last ','
    }
    return query;
}


} // namespace Database
