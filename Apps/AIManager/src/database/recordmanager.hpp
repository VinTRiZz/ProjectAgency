#pragma once

#include <optional>
#include <string>
#include <memory>

#include <ProjectAgency/DB/AIRole.h>
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
    // CRUD operations (simple queries)
    enum class QueryType
    {
        Insert = 0,
        SelectOne,
        SelectAll,
        Delete,
        Update,
    };

public:
    explicit RecordManager();
    ~RecordManager();

    // For outer initialization
    DBConnection& getConnection();

    template <typename IdT, bool isSync = true>
    bool addRecord(RecordBase<IdT>&& iValue) {
        auto res = m_connection.executeQuery(makeSimpleQuery(QueryType::Insert, iValue));
        return (res.has_value());
    }

    template <typename IdT, bool isSync = true>
    bool updateRecord(RecordBase<IdT>&& iValue) {
        auto res = m_connection.executeQuery(makeSimpleQuery(QueryType::Update, iValue));
        return (res.has_value());
    }

    template <typename IdT, bool isSync = true>
    bool removeRecord(RecordBase<IdT>&& iValue) {
        auto res = m_connection.executeQuery(makeSimpleQuery(QueryType::Delete, iValue));
        return (res.has_value());
    }

    template <typename RecordT, bool isSync = true>
    std::optional<RecordT> getRecord(const typename RecordT::id_t& recordId) const {
        RecordT iValue;
        iValue.setId(recordId);
        auto res = m_connection.executeQuery(makeSimpleQuery(QueryType::SelectOne, iValue));
        if (!res.has_value() || !res.initFromRecord(res.value())) {
            return {};
        }
        return iValue;
    }

    template <typename RecordT, bool isSync = true>
    std::vector<RecordT> getAllRecords() const {
        std::vector<RecordT> outputValues;
        RecordT singleValue;
        auto res = m_connection.executeQuery(makeSimpleQuery(QueryType::SelectAll, singleValue));
        if (!res.has_value()) {
            return {};
        }
        for (auto& rec : res.value()) {
            if (!singleValue.initFromRecord(rec)) {
                continue;
            }
            outputValues.push_back(singleValue);
        }
        return outputValues;
    }

private:
    // Connection info
    DBConnection m_connection;

    // Value conversions for simplicity
    std::string recordToColumns(const record_t& rec) const;
    std::string recordToValues(const record_t& rec) const;
    std::string recordToValueAssignList(const record_t& rec) const;

    template <typename IdT>
    std::string makeSimpleQuery(QueryType typ, const RecordBase<IdT>& iRecord) const {
        auto recordV = iRecord.toRecord();
        switch (typ)
        {
        case QueryType::Insert:
            return std::string("INSERT INTO ") +
                iRecord.getTable().data() +
                + " (" + recordToColumns(recordV) + ") VALUES ("
                + recordToValues(recordV) + ")"
            ;
        case QueryType::SelectOne:
            return std::string("SELECT ")
                   + recordToColumns(recordV) + " FROM "
                   + iRecord.getTable().data() +
                   + " WHERE "
                   + iRecord.getIdColumn().data() + " = " + iRecord.getIdString()
                ;
        case QueryType::SelectAll:
            return std::string("SELECT ")
                   + recordToColumns(recordV) + " FROM "
                   + iRecord.getTable().data()
                   + " ORDER BY " + iRecord.getIdColumn().data() + " ASC"
                ;
        case QueryType::Delete:
            return std::string("DELETE FROM ") +
                   iRecord.getTable().data() +
                   + " WHERE "
                   + iRecord.getIdColumn().data() + " = " + iRecord.getIdString()
                ;
        case QueryType::Update:
            return std::string("UPDATE ") +
                   iRecord.getTable().data() +
                   + " SET " + recordToValueAssignList(recordV)
                   + " WHERE "
                   + iRecord.getIdColumn().data() + " = " + iRecord.getIdString()
                ;
        }
        throw std::invalid_argument("RecordManager: Invalid query type passed");
    }
};

} // namespace Database
