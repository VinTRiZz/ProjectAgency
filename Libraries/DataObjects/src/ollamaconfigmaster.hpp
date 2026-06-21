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
    OllamaConfigPtr loadConfig(const std::string& filepath);
    bool saveConfig(const OllamaConfig& config, const std::string& filepath);

    static OllamaConfigPtr createConfig(const std::string& modelType);
    static OllamaConfigPtr fromText(const std::string &content);
};

}
