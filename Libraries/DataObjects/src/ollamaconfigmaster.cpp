#include "ollamaconfigmaster.hpp"

#include <fstream>
#include <string>
#include <regex>

#include "ollamadeepseekconfig.hpp"
#include "ollamaqwenconfig.hpp"

namespace DataObjects
{

class OllamaConfigMaster::Impl {
public:
    std::unique_ptr<OllamaConfig> LoadConfig(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        // Определяем тип модели по содержимому
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

    void SaveConfig(const OllamaConfig& config, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create file: " + filepath);
        }
        file << config.toModelfileString();
    }
};

OllamaConfigMaster::OllamaConfigMaster() : m_Impl(std::make_unique<Impl>()) {}
OllamaConfigMaster::~OllamaConfigMaster() = default;

std::shared_ptr<OllamaConfig> OllamaConfigMaster::loadConfig(const std::string& filepath) {
    return m_Impl->LoadConfig(filepath);
}

void OllamaConfigMaster::saveConfig(const OllamaConfig& config, const std::string& filepath) {
    m_Impl->SaveConfig(config, filepath);
}

std::shared_ptr<OllamaConfig> OllamaConfigMaster::createConfig(const std::string& modelType) {
    if (modelType.find("qwen") != std::string::npos) {
        return std::make_unique<OllamaQwenConfig>();
    } else if (modelType.find("deepseek") != std::string::npos) {
        return std::make_unique<OllamaDeepSeekConfig>();
    } else {
        return std::make_unique<OllamaConfig>();
    }
}

}
