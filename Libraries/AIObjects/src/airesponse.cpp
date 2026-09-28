#include "airesponse.hpp"

#include <nlohmann/json.hpp>

#include <boost/algorithm/string.hpp>

#include <Components/Logger/Logger.h>

namespace AIObjects {

std::string AIResponse::toJson() const
{
    nlohmann::json j;
    j["model"]          = m_modelName;
    j["response"]       = m_response;
    j["created_at"]     = m_timestamp;
    j["done_reason"]    = m_doneReason;
    j["context"]        = m_context;
    j["done"]           = m_isDone;
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

        m_modelName     = parsedJson.value("model", std::string());
        m_thinking      = parsedJson.value("thinking", std::string());
        m_response      = parsedJson.value("response", std::string());
        m_timestamp     = parsedJson.value("created_at", std::string());
        m_doneReason    = parsedJson.value("done_reason", std::string());
        m_isDone        = parsedJson.value("done", true);

        auto ctx = parsedJson["context"];
        m_context.clear();
        m_context.reserve(ctx.size());
        for (auto& v : ctx) {
            m_context.push_back(v);
        }

        return true;
    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_ERROR("[AIResponse] Parsing error:", ex.what());
        COMPLOG_DEBUG("DATA:", std::string(iString.data()));
    }
    return false;
}

std::string AIResponse::getModelName() const
{
    return m_modelName;
}

std::string AIResponse::getThinking() const
{
    return m_thinking;
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

std::vector<uint32_t> AIResponse::getContext() const
{
    return m_context;
}

} // namespace AIObjects
