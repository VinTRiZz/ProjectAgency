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
    DBConnection(const std::string& appName, const std::string& connectionName);


    void setServer(const std::string &address, uint16_t port);

    void setDatabase(const std::string &databaseName);

    void setUser(const std::string &username, const std::string &password);

    void init();
    std::string createConnectionString() const;
    std::vector<record_t> executeQuery(const std::string& queryStr, bool isSync = true) const;
    std::string cellDataToString(const recordValue_t &val) const;



private:
    std::string m_appName;
    std::string m_connectionName;
    std::string m_address;
    uint16_t    m_port;
    std::string m_databaseName;
    std::string m_username;
    std::string m_password;

    drogon::orm::DbClientPtr m_pClient;
};

} // namespace Database
