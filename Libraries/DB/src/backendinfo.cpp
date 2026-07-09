#include "backendinfo.hpp"

#include <stdexcept>

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

namespace DBRecords {

AIBackendInfo::AIBackendInfo() :
    Database::RecordBaseS("sch_manager.t_backends", "device") {

}

AIBackendInfoPtr AIBackendInfo::toPointer()
{
    return std::make_shared<AIBackendInfo>(std::move(*this));
}

AIBackendInfoPtr AIBackendInfo::create(AIBackendInfo&& src)
{
    return std::make_shared<AIBackendInfo>(std::move(src));
}

AIBackendInfoPtr AIBackendInfo::create()
{
    return std::make_shared<AIBackendInfo>();
}

void AIBackendInfo::setId(const std::string &id) noexcept(false)
{
    if (id.size() != 64) {
        throw std::invalid_argument("Invalid id length (expected 64 symbols)");
    }
    Database::RecordBaseS::setId(id);
}

Database::record_t AIBackendInfo::toRecord() const
{
    auto res = Database::RecordBaseS::toRecord();
    res["type"] = m_type;
    res["ip_addr"] = m_ip;
    res["ip_port"] = m_port;
    res["display_name"] = std::string("0x") +Encryption::encodeHex(m_displayName);
    return res;
}

bool AIBackendInfo::initFromRecord(const Database::record_t &iRecord)
{
    auto initRes = Database::RecordBaseS::initFromRecord(iRecord);
    if (!initRes) {
        return false;
    }

    auto colIt = iRecord.find("type");
    if (iRecord.end() == colIt) {
        return false;
    }
    m_type = AIBackendDeviceType(std::get<int64_t>(colIt->second));

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

void AIBackendInfo::setType(AIBackendDeviceType typ)
{
    m_type = typ;
}

AIBackendDeviceType AIBackendInfo::getType() const
{
    return m_type;
}

std::string AIBackendInfo::getTypeString() const
{
    switch (m_type)
    {
    case AIBackendDeviceType::Default:
        return "Computer";
    case AIBackendDeviceType::Android:
        return "Android";
    default:
        throw std::invalid_argument("Invalid device type to get name");
    }
    return {};
}

void AIBackendInfo::setIp(const std::string &ip)
{
    m_ip = ip;
    fixStringValueIssues(m_ip);
}

std::string AIBackendInfo::getIp() const
{
    return m_ip;
}

void AIBackendInfo::setPort(const uint16_t &port)
{
    m_port = port;
}

int64_t AIBackendInfo::getPort() const
{
    return m_port;
}

void AIBackendInfo::setDisplayName(const std::string &displayName)
{
    m_displayName = displayName;
}

std::string AIBackendInfo::getDisplayName() const
{
    return m_displayName;
}

void AIBackendInfo::setToken(const std::string &token)
{
    if (token.size() != 64) {
        throw std::invalid_argument("Invalid token length (expected 64 symbols)");
    }
    m_token = token;
}

std::string AIBackendInfo::getToken() const
{
    return m_token;
}

std::string AIBackendInfo::getFullAddress() const
{
    return m_ip + ":" + std::to_string(m_port);
}

std::string AIBackendInfo::toJson() const
{
    auto cols = toRecord();
    nlohmann::json res;
    for (auto& col : cols) {
        res[col.first] = std::visit([](auto& v) -> nlohmann::json::value_type {
            using valType_t = std::decay_t<decltype(v)>;
            if constexpr(std::is_same_v<valType_t, std::monostate>) {
                return {};
            } else if constexpr(std::is_same_v<valType_t, std::string>) {
                return nlohmann::json::value_type(Encryption::encodeHex(v));
            } else {
                return nlohmann::json::value_type(v);
            }
        }, col.second);
    }
    return res.dump();
}

bool AIBackendInfo::readJson(const std::string &iJson)
{
    try {
        auto iJsonV = nlohmann::json::parse(iJson);
        Database::record_t iRec;
        for (const auto& [key, value] : iJsonV.items()) {
            if (value.is_number_integer()) {
                iRec[key] = int64_t(value);
            } else if (value.is_number_float()) {
                iRec[key] = double(value);
            } else {
                iRec[key] = std::string(Encryption::decodeHex(value)); // Treat anything as a string
            }
        }
        return initFromRecord(iRec);
    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_WARNING("Failed to parse AIBackendInfo:", ex.what());
        return false;
    }
    return true;
}

} // namespace DBRecords
