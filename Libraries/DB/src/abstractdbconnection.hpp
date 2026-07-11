#pragma once

#include <variant>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <optional>
#include <memory>

#include <ProjectAgency/Exchange/Error.h>

namespace Database {

// Common
using recordValue_t = std::variant<std::monostate, std::string, int64_t, double>;
using record_t = std::map<std::string, recordValue_t>;
class AbstractConnection;
using AbstractConnectionPtr = std::shared_ptr<AbstractConnection>;

class AbstractConnection : public Exchange::ErrorUser
{
public:
    AbstractConnection() = default;
    AbstractConnection(const std::string& appName, const std::string& connectionName);

    // Connection properties
    void        setAppName(const std::string& appName);
    std::string getAppName() const;

    void        setName(const std::string& conName);
    std::string getName(const std::string& conName);

    void        setServer(const std::string &address, uint16_t port);
    std::string getServer() const;
    uint16_t    getServerPort() const;

    void        setDatabase(const std::string &databaseName);
    std::string getDatabase() const;

    void setUser(const std::string &username, const std::string &password);
    std::string getUsername() const;
    std::string getPassword() const;

    // Execution working
    using queryCallback_t = std::function<void(std::vector<record_t>&&, const std::string&)>; // arguments: Values and error
    virtual std::optional<std::vector<record_t> > executeQuery(const std::string& queryStr, bool isSync = true) const = 0;
    virtual void executeQueryAsync(const std::string& queryStr, queryCallback_t&& cbk) const = 0;
    virtual std::string cellDataToString(const recordValue_t &val) const;

private:
    std::string m_appName;
    std::string m_connectionName;
    std::string m_address;
    uint16_t    m_port;
    std::string m_databaseName;
    std::string m_username;
    std::string m_password;
};

} // namespace Database
