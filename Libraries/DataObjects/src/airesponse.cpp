#include "airesponse.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>

namespace DataObjects {

std::string AIResponse::toJson() const
{
    nlohmann::json j;
    j["model"] = m_modelName;
    j["response"]   = m_response;
    j["created_at"]  = m_timestamp;
    j["done_reason"] = m_doneReason;
    j["context"]    = m_context;
    return j.dump();
}

bool AIResponse::readJson(const std::string_view &iString)
{
    if (iString.size() < 2) { // 2 is size of {}
        COMPLOG_ERROR("[AIResponse] Parsing error: empty input");
        return false;
    }

    try {
        auto parsedJson = nlohmann::json::parse(iString);

        m_modelName     = parsedJson["model"];
        m_response      = parsedJson["response"];
        m_timestamp     = parsedJson["created_at"];
        m_doneReason    = parsedJson["done_reason"];
        m_context       = parsedJson["context"];

        return true;
    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_ERROR("[AIResponse] Parsing error:", ex.what());
    }
    return false;
}

std::string AIResponse::getModelName() const
{
    return m_modelName;
}

std::string AIResponse::getResponse() const
{
    return m_response;
}

std::string AIResponse::getTimestamp() const
{
    return m_timestamp;
}

std::string AIResponse::getDoneReason() const
{
    return m_doneReason;
}

std::string AIResponse::getContext() const
{
    return m_context;
}

} // namespace DataObjects
