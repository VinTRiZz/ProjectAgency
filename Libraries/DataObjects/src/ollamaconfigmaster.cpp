#include "ollamaconfigmaster.hpp"

#include <fstream>
#include <string>
#include <regex>

#include "ollamadeepseekconfig.hpp"
#include "ollamaqwenconfig.hpp"

namespace DataObjects
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
    file << config.toModelfileString();
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

    config->fromModelfileString(content);
    return config;
}

}
