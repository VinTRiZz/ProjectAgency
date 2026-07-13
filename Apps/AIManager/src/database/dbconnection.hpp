#pragma once

#include <drogon/drogon.h>

#include <variant>
#include <vector>
#include <string>

#include <Components/Database/AbstractConnection.h>

namespace Database {

// Common
using recordValue_t = std::variant<std::monostate, std::string, int64_t, double>;
using DBRowNamed = std::map<std::string, recordValue_t>;

class DBConnection : public AbstractConnection
{
public:
    using AbstractConnection::AbstractConnection;

    void init();

    // Execution working
    std::optional<std::vector<DBRowNamed> > executeQuery(const std::string& queryStr, bool isSync = true) const override;
    void executeQueryAsync(const std::string& queryStr, queryCallback_t&& cbk) const override;

private:
    // Drogon ORM configuration
    std::string createConnectionString() const;
    drogon::orm::DbClientPtr m_pClient;
};

} // namespace Database
