#include "ollamadeepseekconfig.hpp"

#include <nlohmann/json.hpp>

namespace DataObjects {

OllamaDeepSeekConfig::OllamaDeepSeekConfig() {
    m_model.name = "deepseek-r1";
}

std::string OllamaDeepSeekConfig::toJson() const
{
    auto res = OllamaConfig::toJson();
    auto js = nlohmann::json::parse(res); // Think about this?

    auto& spec = js["specific"];
        spec["enableThinking"] = m_specific.enableThinking;

    return js.dump();
}

bool OllamaDeepSeekConfig::readJson(const std::string_view &iString)
{
    if (!OllamaConfig::readJson(iString)) {
        return false;
    }

    // Must have no exception here
    auto configJson = nlohmann::json::parse(iString);
    auto& spec = configJson["specific"];
        m_specific.enableThinking = spec.value("enableThinking", false);
    return true;
}

} // namespace DataObjects
