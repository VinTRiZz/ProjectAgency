#pragma once

#include <drogon/drogon.h>

#include <variant>
#include <vector>
#include <string>

namespace Database {

// Common
using recordValue_t = std::variant<std::monostate, std::string, int64_t, double>;
using record_t = std::map<std::string, recordValue_t>;

class DBConnection
{
public:
    DBConnection() = default;
    DBConnection(const std::string& appName, const std::string& connectionName);

    // Connection properties
    void setAppName(const std::string& appName);
    void setName(const std::string& conName);
    void setServer(const std::string &address, uint16_t port);
    void setDatabase(const std::string &databaseName);
    void setUser(const std::string &username, const std::string &password);
    void init();

    // Execution working
    using queryCallback_t = std::function<void(std::vector<record_t>&&, const std::string&)>; // arguments: Values and error
    std::optional<std::vector<record_t> > executeQuery(const std::string& queryStr, bool isSync = true) const;
    void executeQueryAsync(const std::string& queryStr, queryCallback_t&& cbk) const;
    std::string cellDataToString(const recordValue_t &val) const;

private:
    std::string m_appName;
    std::string m_connectionName;
    std::string m_address;
    uint16_t    m_port;
    std::string m_databaseName;
    std::string m_username;
    std::string m_password;

    // Drogon ORM configuration
    std::string createConnectionString() const;
    drogon::orm::DbClientPtr m_pClient;
};

} // namespace Database
