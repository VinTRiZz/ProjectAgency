#include "ollamaqwenconfig.hpp"

#include <nlohmann/json.hpp>

namespace AIObjects {

OllamaQwenConfig::OllamaQwenConfig() {
    m_model.name = "qwen3.5";
}


std::string OllamaQwenConfig::toJson() const
{
    auto res = OllamaConfig::toJson();
    auto js = nlohmann::json::parse(res); // Think about this?

    auto& spec = js["specific"];
    spec["enableThinking"] = m_specific.enableThinking;

    return js.dump();
}

bool OllamaQwenConfig::readJson(const std::string_view &iString)
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

} // namespace AIObjects
