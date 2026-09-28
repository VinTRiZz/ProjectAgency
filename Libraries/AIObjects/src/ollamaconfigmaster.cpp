#include "ollamaconfigmaster.hpp"

#include <fstream>
#include <string>
#include <regex>

#include <Components/Logger/Logger.h>

#include "ollamadeepseekconfig.hpp"
#include "ollamaqwenconfig.hpp"

namespace AIObjects
{

OllamaConfigPtr OllamaConfigMaster::loadConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return {};
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    return fromText(content);
}

bool OllamaConfigMaster::saveConfig(const OllamaConfig& config, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    file << config.toJson();
    return true;
}

OllamaConfigPtr OllamaConfigMaster::createConfig(const std::string& modelType) {
    if (modelType.find("qwen") != std::string::npos) {
        return std::make_unique<OllamaQwenConfig>();
    } else if (modelType.find("deepseek") != std::string::npos) {
        return std::make_unique<OllamaDeepSeekConfig>();
    }
    return std::make_unique<OllamaConfig>();
}

OllamaConfigPtr OllamaConfigMaster::fromText(const std::string &content)
{
    std::regex fromRe(R"(FROM\s+(\S+))", std::regex::icase);
    std::smatch match;
    std::string modelName;
    if (std::regex_search(content, match, fromRe)) {
        modelName = match[1];
    }

    std::unique_ptr<OllamaConfig> config;
    if (modelName.find("qwen") != std::string::npos) {
        config = std::make_unique<OllamaQwenConfig>();
    } else if (modelName.find("deepseek") != std::string::npos) {
        config = std::make_unique<OllamaDeepSeekConfig>();
    } else {
        config = std::make_unique<OllamaConfig>();
    }

    if (!modelName.empty()) {
        config->m_model.name = modelName;
    }

    std::regex paramRe(R"(PARAMETER\s+(\S+)\s+(.+))", std::regex::icase);
    auto begin = std::sregex_iterator(content.begin(), content.end(), paramRe);
    auto end = std::sregex_iterator();

    std::string key;
    std::string value;
    try {
        for (auto it = begin; it != end; ++it) {
            key = (*it)[1];
            value = (*it)[2];
            if (key == "temperature") {
                config->m_sampling.temperature = std::stof(value);
            } else if (key == "top_p") {
                config->m_sampling.topP = std::stof(value);
            } else if (key == "top_k") {
                config->m_sampling.topK = std::stoi(value);
            } else if (key == "repeat_penalty") {
                config->m_repetition.repeatPenalty = std::stof(value);
            } else if (key == "num_ctx") {
                config->m_model.numCtx = std::stoi(value);
            } else if (key == "num_predict") {
                config->m_model.numPredict = std::stoi(value);
            } else if (key == "stream") {
                config->m_extra.stream = (value == "true");
            } else if (key == "keep_alive") {
                config->m_extra.keepAliveS = std::stoi(value);
            } else if (key == "answer_timeout") {
                config->m_extra.answerTimeoutMs = static_cast<uint32_t>(std::stoul(value));
            } else if (key == "enable_thinking") {
                if (auto* ds = dynamic_cast<OllamaDeepSeekConfig*>(config.get())) {
                    ds->m_specific.enableThinking = (value == "true");
                } else if (auto* qw = dynamic_cast<OllamaQwenConfig*>(config.get())) {
                    qw->m_specific.enableThinking = (value == "true");
                }
            }
        }
    } catch (const std::invalid_argument& ex) {
        COMPLOG_WARNING("OllamaConfigMaster: Invalid value in model config (could not be parsed):", key, "is set to", value);
        return {};
    }

    std::regex systemRe(R"re(SYSTEM\s+"""([\s\S]*?)""")re", std::regex::icase);
    if (std::regex_search(content, match, systemRe)) {
        config->m_model.systemPrompt = match[1];
    }
    return config;
}

void OllamaConfigMaster::configureForOrchestra(const OllamaConfigPtr &pModel)
{
    // Balanced precision: needs to evaluate context and route correctly
    pModel->m_sampling.temperature = 0.2f;
    pModel->m_sampling.topP = 0.9f;
    pModel->m_sampling.topK = 40;

    pModel->m_repetition.repeatPenalty = 1.1f;
}

void OllamaConfigMaster::configureForCode(const OllamaConfigPtr &pModel)
{
    // Low temperature for consistent, deterministic code output
    pModel->m_sampling.temperature = 0.1f;
    pModel->m_sampling.topP = 0.9f;
    pModel->m_sampling.topK = 40;

    pModel->m_repetition.repeatPenalty = 1.05f;
}

void OllamaConfigMaster::configureForPlanning(const OllamaConfigPtr &pModel)
{
    // Moderate temperature allows structured reasoning with some exploration
    pModel->m_sampling.temperature = 0.3f;
    pModel->m_sampling.topP = 0.9f;
    pModel->m_sampling.topK = 40;

    pModel->m_repetition.repeatPenalty = 1.1f;
}

void OllamaConfigMaster::configureForTranslating(const OllamaConfigPtr &pModel)
{
    // Moderate temperature balances fluency and faithfulness to source
    pModel->m_sampling.temperature = 0.5f;
    pModel->m_sampling.topP = 0.9f;
    pModel->m_sampling.topK = 40;

    pModel->m_repetition.repeatPenalty = 1.1f;
}

}
