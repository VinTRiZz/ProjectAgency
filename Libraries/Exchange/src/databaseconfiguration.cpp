#include "databaseconfiguration.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

namespace Exchange {

std::string DatabaseConfiguration::toJson() const
{
    nlohmann::json res;

    res["address"] = Encryption::encodeHex(m_dbAddress);
    res["port"] = m_dbPort;

    res["name"] = Encryption::encodeHex(m_dbName);
    res["user"] = Encryption::encodeHex(m_dbUsername);
    res["pass"] = Encryption::encodeHex(m_dbPassword);

    m_error.setCode(ErrorCode::NoError);
    return res.dump();
}

bool DatabaseConfiguration::readJson(const std::string_view &iString)
{
    try {
        auto dbJson = nlohmann::json::parse(iString);

        m_dbAddress = Encryption::decodeHex(dbJson["address"]);
        m_dbPort = dbJson["port"];

        m_dbName = Encryption::decodeHex(dbJson["name"]);
        m_dbUsername = Encryption::decodeHex(dbJson["user"]);
        m_dbPassword = Encryption::decodeHex(dbJson["pass"]);

        m_error.setCode(ErrorCode::NoError);
    } catch (nlohmann::json::exception& ex) {
        m_error.setCode(ErrorCode::ProtocolJsonException);
        m_error.setDetailText(std::string("DatabaseConfiguration | ") + ex.what());
        return false;
    }
    return true;
}

bool DatabaseConfiguration::operator ==(const DatabaseConfiguration& conf) const {
    return
        (m_dbAddress == conf.m_dbAddress) &&
        (m_dbName == conf.m_dbName) &&
        (m_dbPort == conf.m_dbPort) &&
        (m_dbUsername == conf.m_dbUsername) &&
        (m_dbPassword == conf.m_dbPassword)
    ;
}

bool DatabaseConfiguration::operator !=(const DatabaseConfiguration& conf) const {
    return !(*this == conf);
}

} // namespace Exchange
