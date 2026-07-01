#include "backendinfo.hpp"

#include <stdexcept>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

namespace DBRecords {

BackendInfo::BackendInfo() :
    Database::RecordBaseS("sch_manager.t_backends", "device") {

}

void BackendInfo::setId(const std::string &id) noexcept(false)
{
    if (id.size() != 64) {
        throw std::invalid_argument("Invalid id length (expected 64 symbols)");
    }
    Database::RecordBaseS::setId(id);
}

Database::record_t BackendInfo::toRecord() const
{
    auto res = Database::RecordBaseS::toRecord();
    res["type"] = m_type;
    res["ip_addr"] = m_ip;
    res["ip_port"] = m_port;
    res["display_name"] = std::string("0x") +Encryption::encodeHex(m_displayName);
    return res;
}

bool BackendInfo::initFromRecord(const Database::record_t &iRecord)
{
    auto initRes = Database::RecordBaseS::initFromRecord(iRecord);
    if (!initRes) {
        return false;
    }

    auto colIt = iRecord.find("type");
    if (iRecord.end() == colIt) {
        return false;
    }
    m_type = BackendType(std::get<int64_t>(colIt->second));

    colIt = iRecord.find("ip_addr");
    if (iRecord.end() == colIt) {
        return false;
    }
    m_ip = std::get<std::string>(colIt->second);

    colIt = iRecord.find("ip_port");
    if (iRecord.end() == colIt) {
        return false;
    }
    m_port = std::get<int64_t>(colIt->second);

    colIt = iRecord.find("display_name");
    if (iRecord.end() == colIt) {
        return false;
    }
    try {
        auto hexStr = std::get<std::string>(colIt->second);
        if (hexStr.size() >= 2) {
            hexStr = hexStr.substr(2);
        }
        m_displayName = Encryption::decodeHex(hexStr);
    } catch (const std::exception& ex) {
        COMPLOG_ERROR("Failed to load AIBackend name:", ex.what());
        return false;
    }

    return true;
}

void BackendInfo::setIp(const std::string &ip)
{
    m_ip = ip;
    fixStringValueIssues(m_ip);
}

std::string BackendInfo::getIp() const
{
    return m_ip;
}

void BackendInfo::setPort(const uint16_t &port)
{
    m_port = port;
}

int64_t BackendInfo::getPort() const
{
    return m_port;
}

void BackendInfo::setDisplayName(const std::string &displayName)
{
    m_displayName = displayName;
}

std::string BackendInfo::getDisplayName() const
{
    return m_displayName;
}

void BackendInfo::setToken(const std::string &token)
{
    if (token.size() != 64) {
        throw std::invalid_argument("Invalid token length (expected 64 symbols)");
    }
    m_token = token;
}

std::string BackendInfo::getToken() const
{
    return m_token;
}

std::string BackendInfo::getFullAddress() const
{
    return m_ip + ":" + std::to_string(m_port);
}

std::string BackendInfo::toJson() const
{
    return {};
}

bool BackendInfo::readJson(const std::string &iJson)
{
    return false;
}

} // namespace DBRecords
