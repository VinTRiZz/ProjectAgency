#include "dbconnection.hpp"

#include <Components/Logger/Logger.h>

#include <ProjectAgency/Types.h>

namespace Database {

/**
 * @brief The CellType enum Cell value type. Created because no way in Drogon ORM to get it normal way
 */
enum CellType : int
{
    CT_unknown = 0,
    CT_integer,
    CT_double,
    CT_string
};

/**
 * @brief deduceCellType Issue of Drogon ORM working
 * @param rowCell
 * @return Type of a cell (actually, it's column)
 */
CellType deduceCellType(const drogon::orm::Field& rowCell) {
    if (rowCell.isNull()) { // Can not deduce NULL field type
        return CT_unknown;
    }

    auto strVal = rowCell.as<std::string>();

    // Process HEX
    if (strVal.size() > 2 && strVal[0] == '0' && strVal[1] == 'x') {
        return CellType::CT_string;
    }

    // Try integer
    try {
        std::size_t convEndPos {0};
        std::stol(strVal, &convEndPos);
        if (convEndPos != strVal.length()) {
            throw std::invalid_argument("Not end");
        }
        return CellType::CT_integer;
    } catch (const std::invalid_argument& ex) {
        // Treat as other
    }

    // Try double
    try {
        std::size_t convEndPos {0};
        std::stod(strVal, &convEndPos);
        if (convEndPos != strVal.length()) {
            throw std::invalid_argument("Not end");
        }
        return CellType::CT_double;
    } catch (const std::invalid_argument& ex) {
        // Treat as other
    }

    // Unknown type
    return CellType::CT_string;
}

/**
 * @brief cellToRecord  Convert cell to a record value
 * @param rowCell
 * @param columnType
 * @return
 */
recordValue_t cellToRecord(const drogon::orm::Field& rowCell, CellType columnType) {
    if (rowCell.isNull()) {
        return std::monostate();
    }

    switch (columnType)
    {
    case CellType::CT_integer: return rowCell.as<int64_t>();
    case CellType::CT_double: return rowCell.as<double>();
    case CellType::CT_string: return rowCell.as<std::string>();
    case CellType::CT_unknown: return std::monostate(); // Treat unknown type as NULL (actually, proceed in deduceType function)
    default: break;
    }
    throw std::runtime_error(std::string("Unexpected type of column: ") + std::to_string(columnType));
}

std::vector<record_t> resultToRecords(const drogon::orm::Result& execResult) {

    std::map<int, CellType> columnTypes;

    std::vector<record_t> res;
    for (auto& row : execResult) {
        record_t record;
        for (size_t i = 0; i < row.size(); ++i) {
            // Use deduced type
            if (columnTypes.count(i)) {
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

void DBConnection::setAppName(const std::string &appName)
{
    m_appName = appName;
}

void DBConnection::setName(const std::string &conName)
{
    m_connectionName = conName;
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

std::optional<std::vector<record_t> > DBConnection::executeQuery(const std::string &queryStr, bool isSync) const
{
    std::vector<record_t> res;
    try {
        if (std::string::npos != queryStr.find(';')) {
            throw std::runtime_error("[DBConnection] Invalid query. Expected query with no ';' symbol");
        }
        // COMPLOG_DEBUG("Executing:", queryStr);
        auto execRes = m_pClient->execSqlSync(queryStr);
        res = resultToRecords(execRes);
    } catch (const drogon::orm::DrogonDbException& ex) {
        COMPLOG_ERROR("[DBConnection] SYNC Record exec error:", ex.base().what());
        return {};
    }
    return res;
}

void DBConnection::executeQueryAsync(const std::string &queryStr, queryCallback_t &&cbk) const
{
    try {
        if (std::string::npos != queryStr.find(';')) {
            throw std::runtime_error("[DBConnection] Invalid query. Expected query with no ';' symbol");
        }
        // COMPLOG_DEBUG("Executing:", queryStr);
        auto cbkCopy = cbk;
        m_pClient->execSqlAsync(queryStr,
            [cbk = std::move(cbk)](const drogon::orm::Result& execRes){
            std::vector<record_t> res = resultToRecords(execRes);
            cbk(std::move(res), {});
        },
            [cbk = std::move(cbkCopy)](const drogon::orm::DrogonDbException& ex){
            COMPLOG_ERROR("[DBConnection] ASYNC Record exec error:", ex.base().what());
            cbk({}, ex.base().what());
        });
    } catch (const drogon::orm::DrogonDbException& ex) {
        COMPLOG_ERROR("[DBConnection] ASYNC Record start exec error:", ex.base().what());
    }
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

} // namespace Database
