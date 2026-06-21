#include "ollamaconfig.hpp"

#include <Components/Logger/Logger.h>

#include <nlohmann/json.hpp>

namespace DataObjects {

std::string OllamaConfig::toJson() const {
    nlohmann::json res;

    auto& model = res["model"];
        model["name"]           = m_model.name;
        model["systemPrompt"]   = m_model.systemPrompt;
        model["numPredict"]     = m_model.numPredict;
        model["numCtx"]         = m_model.numCtx;

    auto& sampling = res["sampling"];
        sampling["temperature"] = m_sampling.temperature;
        sampling["topP"] = m_sampling.topP;
        sampling["topK"] = m_sampling.topK;

    auto& repeat = res["repeat"];
        repeat["token"] = m_repetition.repeatPenalty;

    auto& extra = res["extra"];
        extra["stream"]     = m_extra.stream;
        extra["keepAlive"]  = m_extra.keepAlive;
        extra["suffix"]     = m_extra.suffix;

    return res.dump();
}

bool OllamaConfig::readJson(const std::string_view& iString) {
    try {
        auto statusJson = nlohmann::json::parse(iString);

        // Model systems
        auto& modelJson = statusJson.at("model");
            m_model.name           = modelJson.value("name", "");
            m_model.systemPrompt   = modelJson.value("systemPrompt", "");
            m_model.numPredict     = modelJson.value("numPredict", 0);
            m_model.numCtx         = modelJson.value("numCtx", 0);

        // Sample parameters
        auto& samplingJson = statusJson.at("sampling");
            samplingJson["temperature"] = m_sampling.temperature;
            samplingJson["topP"]        = m_sampling.topP;
            samplingJson["topK"]        = m_sampling.topK;

        // Repeat configuration
        auto& repeatJson = statusJson.at("repeat");
            m_repetition.repeatPenalty = repeatJson.value("token", 0.0);

        // Extra parameters
        auto& extraJson = statusJson.at("extra");
            m_extra.stream    = extraJson.value("stream", false);
            m_extra.keepAlive = extraJson.value("keepAlive", 60);
            m_extra.suffix    = extraJson.value("suffix", "");
    } catch (nlohmann::json::exception& ex) {
        COMPLOG_ERROR("OllamaConfig Parse error:", ex.what());
        return false;
    } catch (std::exception& ex) {
        COMPLOG_ERROR("OllamaConfig Error:", ex.what());
        return false;
    }
    return true;
}

}