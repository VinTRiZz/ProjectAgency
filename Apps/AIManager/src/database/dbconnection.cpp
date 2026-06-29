#include "dbconnection.hpp"

#include <ProjectAgency/Types.h>

namespace Database {


int deduceCellType(const drogon::orm::Field& rowCell) {
    auto strVal = row[i].as<std::string>();
    try {
        std::size_t convEndPos {0};
        auto convRes = std::stol(strVal, &convEndPos);
        if (convEndPos != strVal.length()) {
            throw std::invalid_argument("len end not reached");
        }
        record[execResult.columnName(i)] = convRes;
        columnTypes[i] = 1;

    } catch (const std::invalid_argument& ex) {
        try {
            std::size_t convEndPos {0};
            auto doubleV = std::stod(strVal, &convEndPos);
            if (convEndPos != strVal.length()) {
                throw std::invalid_argument("len end not reached");
            }
            record[execResult.columnName(i)] = doubleV;
            columnTypes[i] = 2;

        } catch (const std::invalid_argument& ex) {
            record[execResult.columnName(i)] = strVal;
            columnTypes[i] = 3;
        }
    }
}

recordValue_t cellToRecord(const drogon::orm::Field& rowCell, int columnType) {
    recordValue_t res { std::monostate() };
    if (rowCell.isNull()) {
        return res;
    } else {
        switch (columnType)
        {
        case 1: return rowCell.as<int64_t>();       // Integer
        case 2: return rowCell.as<double>();        // double prec
        case 3: return rowCell.as<std::string>();   // Other type (treat as string)
        }
    }
    throw std::runtime_error(std::string("Unexpected type of column: ") + std::to_string(columnType));
}

std::vector<record_t> resultToRecords(const drogon::orm::Result& execResult) {

    std::map<int, int> columnTypes; // For cast easier

    std::vector<record_t> res;
    for (auto& row : execResult) {
        record_t record;
        for (size_t i = 0; i < row.size(); ++i) {
            // Use deduced type
            if (columnTypes[i] != 0) {
                record[execResult.columnName(i)] = cellToRecord(row[i], columnTypes[i]);
                continue;
            }

            if (row[i].isNull()) {
                record[execResult.columnName(i)] = {};
                continue;
            }

            // Deduce type
            auto colType = deduceCellType(row[i]);
            columnTypes[i] = colType;
            record[execResult.columnName(i)] = cellToRecord(row[i], colType);
        }
        res.push_back(record);
    }
    return res;
}



DBConnection::DBConnection(const std::string &appName, const std::string &connectionName) :
    m_appName { appName },
    m_connectionName{ connectionName } {

}

void DBConnection::setServer(const std::string &address, uint16_t port)
{
    m_address = address;
    m_port = port;
}

void DBConnection::setDatabase(const std::string &databaseName)
{
    m_databaseName = databaseName;
}

void DBConnection::setUser(const std::string &username, const std::string &password)
{
    m_username = username;
    m_password = password;
}

void DBConnection::init()
{
    m_pClient = drogon::orm::DbClient::newPgClient(createConnectionString(), 2);
}

std::string DBConnection::createConnectionString() const
{
    // "host=127.0.0.1 port=5432 dbname=test user=user password=pass"
    std::string connString = "host=" + m_address +
                             " port=" + std::to_string(m_port) +
                             " dbname=" + m_databaseName +
                             " user=" + m_username +
                             " password=" + m_password +
                             " application_name=" + m_appName;
    return connString;
}

std::vector<record_t> DBConnection::executeQuery(const std::string &queryStr, bool isSync) const
{
    std::vector<record_t> res;
    try {
        if (std::string::npos != queryStr.find(';')) {
            throw std::runtime_error("Invalid query. Expected query with no ';' symbol");
        }
        auto execRes = m_pClient->execSqlSync(queryStr);
        res = resultToRecords(execRes);
    } catch (const drogon::orm::DrogonDbException& ex) {
        COMPLOG_ERROR("[RecordManager] SYNC Record exec error:", ex.base().what());
        return {};
    }
    return res;
}

std::string DBConnection::cellDataToString(const recordValue_t &val) const
{
    return std::visit([](auto& v) -> std::string {
        using valueType_t = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<valueType_t, std::string>) {
            return v;
        } else
            if constexpr (std::is_same_v<valueType_t, DataObjects::id_t>) {
                if (v == DataObjects::NULL_ID) {
                    return "NULL";
                }
                return std::to_string(v);
            } else
                if constexpr (std::is_same_v<valueType_t, double>) {
                    return std::to_string(v);
                }
        return {};
    }, val);
}

} // namespace Database
