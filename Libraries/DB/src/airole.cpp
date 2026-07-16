#include "airole.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

#include <ProjectAgency/Exchange/Error.h>

namespace DBRecords {

AIRole::AIRole() :
    Database::RecordBaseI("sch_manager.t_model_roles") {

}

Database::DBRowNamed AIRole::toRecord() const
{
    auto res = Database::RecordBaseI::toRecord();
    res["version"] = m_version;
    res["name"] = std::string("0x") + Encryption::encodeHex(m_name);
    res["type"] = m_type;
    res["config"] = m_configJson;
    return res;
}

bool AIRole::initFromRecord(const Database::DBRowNamed &iRecord)
{
    Database::RecordBaseI::initFromRecord(iRecord);

    auto colIt = iRecord.find("version");
    if (iRecord.end() == colIt) {
        m_error.setCode(Exchange::ErrorCode::ProtocolInvalidData);
        m_error.setDetailText("No column: version");
        return false;
    }
    m_version = std::get<int64_t>(colIt->second);

    colIt = iRecord.find("name");
    if (iRecord.end() == colIt) {
        m_error.setCode(Exchange::ErrorCode::ProtocolInvalidData);
        m_error.setDetailText("No column: name");
        return false;
    }
    try {
        auto hexStr = std::get<std::string>(colIt->second);
        if (hexStr.size() >= 2) {
            hexStr = hexStr.substr(2);
        }
        m_name = Encryption::decodeHex(hexStr);
    } catch (const std::exception& ex) {
        m_error.setCode(Exchange::ErrorCode::ProtocolInvalidData);
        m_error.setDetailText(ex.what());
        return false;
    }

    colIt = iRecord.find("type");
    if (iRecord.end() == colIt) {
        m_error.setCode(Exchange::ErrorCode::ProtocolInvalidData);
        m_error.setDetailText("No column: type");
        return false;
    }
    m_type = (std::holds_alternative<std::monostate>(colIt->second) ? std::string() : std::get<std::string>(colIt->second));

    colIt = iRecord.find("config");
    if (iRecord.end() == colIt) {
        m_error.setCode(Exchange::ErrorCode::ProtocolInvalidData);
        m_error.setDetailText("No column: config");
        return false;
    }
    m_configJson = std::get<std::string>(colIt->second);

    m_error.reset();
    return true;
}

void AIRole::setVersion(unsigned int version)
{
    m_version = version;
}

unsigned int AIRole::getVersion() const
{
    return m_version;
}

void AIRole::setName(const std::string &name)
{
    m_name = name;
}

std::string AIRole::getName() const
{
    return m_name;
}

void AIRole::setType(const std::string &type)
{
    m_type = type;
    fixStringValueIssues(m_type);
}

std::string AIRole::getType() const
{
    return m_type;
}

void AIRole::setConfig(const std::string &configJson)
{
    m_configJson = configJson;
}

std::string AIRole::getConfig() const
{
    return m_configJson;
}

std::string AIRole::toJson() const
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

bool AIRole::fromJson(const std::string &iJson)
{
    try {
        auto iJsonV = nlohmann::json::parse(iJson);
        Database::DBRowNamed iRec;
        for (const auto& [key, value] : iJsonV.items()) {
            if (value.is_number_integer()) {
                iRec[key] = int64_t(value);
            } else if (value.is_number_float()) {
                iRec[key] = double(value);
            } else if (value.is_null()) {
                iRec[key] = std::monostate();
            } else {
                iRec[key] = std::string(Encryption::decodeHex(value)); // Treat anything as a string
            }
        }
        m_error.reset();
        return initFromRecord(iRec);
    } catch (const nlohmann::json::exception& ex) {
        m_error.setCode(Exchange::ErrorCode::ProtocolJsonException);
        m_error.setDetailText(std::string("AIRole | ") + ex.what());
        return false;
    }
    return true;
}

} // namespace DBRecords
