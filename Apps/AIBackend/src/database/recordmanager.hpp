#pragma once

#include <memory>
#include <type_traits>

#include <ProjectAgency/Types.h>

#include <Components/Database/SQlite.h>

namespace Database {

class RecordBase;

template <
    typename RecordT,
    typename = std::void_t<std::is_base_of<RecordBase, RecordT> > >
class RecordManager
{
public:
    explicit RecordManager(SQLiteDatabase& db) : m_db {db} {

    }

    bool addRecord(RecordT&& iValue) {
        return m_recordsTable.addRow(iValue.toRecord());
    }

    RecordT getRecord(DataObjects::id_t recId) {

    }

    bool updateRecord(RecordT&& iValue) {

    }

    bool removeRecord(RecordT&& iValue) {

    }

private:
    Database::SQLiteDatabase& m_db;
    Database::SQLiteTable m_recordsTable {m_db};
};

template <typename RecordT>
using RecordManagerPtr = std::shared_ptr<RecordManager<RecordT> >;

} // namespace Database
