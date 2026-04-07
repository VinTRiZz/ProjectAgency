#include "airesponse.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>

namespace DataObjects {

std::string AIResponse::toJson() const
{
    return {};
}

bool AIResponse::readJson(const std::string &iString)
{
    try {
        auto parsedJson = nlohmann::json::parse(iString);

        m_modelName     = parsedJson["model"];
        m_response      = parsedJson["response"];
        m_timestamp     = parsedJson["timestamp"];
        m_doneReason    = parsedJson["done_reason"];
        m_context       = parsedJson["context"];

        return true;
    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_ERROR("[WSEvent] Parsing error:", ex.what());
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
