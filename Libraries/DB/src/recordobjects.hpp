#pragma once

#include <string>
#include <algorithm>
#include <variant>
#include <map>

#include <ProjectAgency/Types.h>

namespace Database {

// Common
using recordValue_t = std::variant<std::monostate, std::string, int64_t, double>;
using record_t = std::map<std::string, recordValue_t>;

/**
 * @brief The RecordBase class Basic class for converting from/to DB records
 */
template <typename IdT>
class RecordBase {
public:
    using id_t = IdT;

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

    virtual bool initFromRecord(const record_t& iRecord) {
        auto idColIt = iRecord.find(m_idColumnName);
        if (iRecord.end() == idColIt) {
            return false;
        }
        if constexpr (std::is_arithmetic_v<IdT> || std::is_same_v<IdT, DataObjects::id_t>) {
            m_id = std::get<int64_t>(idColIt->second);
        } else {
            m_id = std::get<std::string>(idColIt->second);
        }
        return true;
    }

    virtual void setId(const IdT& id) { m_id = id; }
    IdT getId() const { return m_id; }
    virtual std::string getIdString() const {
        if constexpr (std::is_same_v<IdT, std::string>) {
            return m_id;
        } else if constexpr (std::is_same_v<IdT, DataObjects::id_t>) {
            return std::to_string(m_id);
        }
        return {};
    }

private:
    std::string m_table;
    std::string m_idColumnName;
    IdT m_id {};

protected:
    // Clean string from symbols like ', ;, etc.
    void fixStringValueIssues(std::string& inputStr) const {
        std::replace_if(inputStr.data(), inputStr.data() + inputStr.size(),
                        [](auto c){
            return (c == '\'') || (c == ';') || (c == '"');
        }, ' ');
    }
};

using RecordBaseI = RecordBase<DataObjects::id_t>;
using RecordBaseS = RecordBase<std::string>;

} // namespace Database
