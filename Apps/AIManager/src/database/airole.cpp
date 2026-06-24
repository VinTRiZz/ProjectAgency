#include "airole.hpp"

namespace DBRecords {

AIRole::AIRole() :
    Database::RecordBaseI("sch_manager.t_model_roles") {

}

Database::record_t AIRole::toRecord() const
{
    auto res = Database::RecordBaseI::toRecord();
    res["version"] = m_version;
    res["name"] = m_name;
    res["type"] = m_type;
    res["config"] = m_configJson;
    return res;
}

bool AIRole::initFromRecord(const Database::record_t &iRecord)
{
    *this = {}; // erase self values
    Database::RecordBaseI::initFromRecord(iRecord);

    auto colIt = iRecord.find("version");
    if (iRecord.end() == colIt) {
        return false;
    }
    m_version = std::get<int64_t>(colIt->second);

    colIt = iRecord.find("name");
    if (iRecord.end() == colIt) {
        return false;
    }
    m_name = std::get<std::string>(colIt->second);

    colIt = iRecord.find("type");
    if (iRecord.end() == colIt) {
        return false;
    }
    m_type = std::get<std::string>(colIt->second);

    colIt = iRecord.find("config");
    if (iRecord.end() == colIt) {
        return false;
    }
    m_configJson = std::get<std::string>(colIt->second);

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
    fixStringValueIssues(m_name);
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
    return m_name;
}

void AIRole::setConfig(const std::string &configJson)
{
    m_configJson = configJson;
}

std::string AIRole::getConfig() const
{
    return m_configJson;
}

} // namespace DBRecords
