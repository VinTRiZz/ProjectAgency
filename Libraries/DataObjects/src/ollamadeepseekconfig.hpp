#pragma once

#include "ollamaconfig.hpp"

namespace DataObjects {

/**
 * @brief The OllamaDeepSeekConfig class Configuration for DeepSeek-R1 mainly, but can be used for DeepSeek line also
 */
class OllamaDeepSeekConfig : public OllamaConfig {
public:
    OllamaDeepSeekConfig();

    void setReasoningEffort(const std::string& level);
};

} // namespace DataObjects
