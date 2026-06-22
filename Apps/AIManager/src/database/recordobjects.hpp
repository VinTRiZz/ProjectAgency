#pragma once

#include <string>
#include <variant>
#include <map>

#include <ProjectAgency/Types.h>

namespace Database {

// Common
using recordValue_t = std::variant<std::string, int64_t, double>;
using record_t = std::map<std::string, recordValue_t>;

/**
 * @brief The RecordBase class Basic class for converting from/to DB records
 */
template <typename IdT>
class RecordBase {
public:
    explicit RecordBase(const std::string& tableName, const std::string& idColumn = "id") {
        m_table = tableName;
        m_idColumnName = idColumn;
    }

    std::string_view getTable() const {
        return m_table;
    }
    std::string_view getIdColumn() const {
        return m_idColumnName;
    }

    virtual record_t toRecord() const {
        record_t res;
        res[m_idColumnName] = m_id;
        return res;
    }

    virtual void initFromRecord(const record_t& iRecord) {
        if constexpr (std::is_arithmetic_v<IdT> || std::is_same_v<IdT, DataObjects::id_t>) {
            m_id = std::get<int64_t>(iRecord.at(m_idColumnName));
        } else {
            m_id = std::get<std::string>(iRecord.at(m_idColumnName));
        }
    }

    void setId(DataObjects::id_t id);
    DataObjects::id_t getId() const;

private:
    std::string m_table;
    std::string m_idColumnName;
    IdT m_id {};
};

using RecordBaseI = RecordBase<DataObjects::id_t>;
using RecordBaseS = RecordBase<std::string>;

} // namespace Database
