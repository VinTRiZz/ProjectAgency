#include "ollamaconfigmaster.hpp"

#include <fstream>
#include <string>
#include <regex>

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

    if (!config->readJson(content)) {
        return {};
    }
    return config;
}

void OllamaConfigMaster::configureForOrchestra(const OllamaConfigPtr &pModel)
{
    // More strict, but with exprompts
    pModel->m_sampling.temperature = 0.01f;
    pModel->m_sampling.topP = 0.1f;
    pModel->m_sampling.topK = 5;

    pModel->m_repetition.repeatPenalty = 0.6f;
}

void OllamaConfigMaster::configureForCode(const OllamaConfigPtr &pModel)
{
    // Strict, style-following, no extra random
    pModel->m_sampling.temperature = 0.005f;
    pModel->m_sampling.topP = 0.5f;
    pModel->m_sampling.topK = 5;

    pModel->m_repetition.repeatPenalty = 0.5f;
}

void OllamaConfigMaster::configureForPlanning(const OllamaConfigPtr &pModel)
{
    // More exprompts, but follow target
    pModel->m_sampling.temperature = 0.3f;
    pModel->m_sampling.topP = 0.1f;
    pModel->m_sampling.topK = 2;

    pModel->m_repetition.repeatPenalty = 0.6f;
}

void OllamaConfigMaster::configureForTranslating(const OllamaConfigPtr &pModel)
{
    // Creativeness and rechecking is the key
    pModel->m_sampling.temperature = 1.0f;
    pModel->m_sampling.topP = 0.4f;
    pModel->m_sampling.topK = 10;

    pModel->m_repetition.repeatPenalty = 1.4f;
}

}
