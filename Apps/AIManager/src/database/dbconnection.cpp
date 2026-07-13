#include "dbconnection.hpp"

#include <Components/Logger/Logger.h>

#include <ProjectAgency/Exchange/Types.h>
#include <ProjectAgency/Exchange/Error.h>

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
    throw Exchange::Error(Exchange::ErrorCode::SystemInvalidArgument, "Unexpected column type");
}

std::vector<DBRowNamed> resultToRecords(const drogon::orm::Result& execResult) {

    std::map<int, CellType> columnTypes;

    std::vector<DBRowNamed> res;
    for (auto& row : execResult) {
        DBRowNamed record;
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

void DBConnection::init()
{
    m_pClient = drogon::orm::DbClient::newPgClient(createConnectionString(), 2);
}

std::optional<std::vector<DBRowNamed> > DBConnection::executeQuery(const std::string &queryStr, bool isSync) const
{
    std::vector<DBRowNamed> res;
    try {
        if (std::string::npos != queryStr.find(';')) {
            throw Exchange::Error(Exchange::ErrorCode::SystemDBError, "Invalid query (contain ';' symbol)");
        }
        // COMPLOG_DEBUG("Executing:", queryStr);
        auto execRes = m_pClient->execSqlSync(queryStr);
        res = resultToRecords(execRes);
        m_error.reset();
    } catch (const drogon::orm::DrogonDbException& ex) {
        m_error.setCode(Exchange::ErrorCode::SystemDBError);
        m_error.setDetailText(ex.base().what());
        return {};
    }
    return res;
}

void DBConnection::executeQueryAsync(const std::string &queryStr, queryCallback_t &&cbk) const
{
    m_error.reset();
    try {
        if (std::string::npos != queryStr.find(';')) {
            throw Exchange::Error(Exchange::ErrorCode::SystemDBError, "Invalid query (contain ';' symbol)");
        }
        // COMPLOG_DEBUG("Executing:", queryStr);
        auto cbkCopy = cbk;
        m_pClient->execSqlAsync(queryStr,
            [cbk = std::move(cbk)](const drogon::orm::Result& execRes){
            std::vector<DBRowNamed> res = resultToRecords(execRes);
            cbk(std::move(res), {});
        },
            [cbk = std::move(cbkCopy)](const drogon::orm::DrogonDbException& ex){
            Exchange::Error::printSelf(Exchange::ErrorCode::SystemDBError, ex.base().what());
            cbk({}, ex.base().what());
        });
    } catch (const drogon::orm::DrogonDbException& ex) {
        m_error.setCode(Exchange::ErrorCode::SystemDBError);
        m_error.setDetailText(ex.base().what());
    }
}

std::string DBConnection::createConnectionString() const
{
    // "host=127.0.0.1 port=5432 dbname=test user=user password=pass"
    std::string connString = "host=" + getServer() +
                             " port=" + std::to_string(getServerPort()) +
                             " dbname=" + getDatabase() +
                             " user=" + getUsername() +
                             " password=" + getPassword() +
                             " application_name=" + getAppName();
    return connString;
}

} // namespace Database
