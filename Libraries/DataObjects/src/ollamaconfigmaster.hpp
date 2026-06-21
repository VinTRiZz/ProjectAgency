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

    // Configuration presets (such as temperature)
    static void configureForOrchestra(const OllamaConfigPtr& pModel);   // Orchestrator model
    static void configureForCode(const OllamaConfigPtr& pModel);        // Coding model
    static void configureForPlanning(const OllamaConfigPtr& pModel);    // Planner of task solving
    static void configureForTranslating(const OllamaConfigPtr& pModel); // Translator of user requests
};

}
