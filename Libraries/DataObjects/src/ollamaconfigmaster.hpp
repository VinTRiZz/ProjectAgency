#pragma once

#include <string>
#include <memory>

#include "ollamaconfig.hpp"

namespace DataObjects
{

/**
 * @brief The OllamaConfigMaster class Class for generating and loading configurations of models
 */
class OllamaConfigMaster {
public:
    std::shared_ptr<OllamaConfig> loadConfig(const std::string& filepath);
    bool saveConfig(const OllamaConfig& config, const std::string& filepath);

    static std::shared_ptr<OllamaConfig> createConfig(const std::string& modelType);
    static std::shared_ptr<OllamaConfig> fromText(const std::string &content);
};

}
