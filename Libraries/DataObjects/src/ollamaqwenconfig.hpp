#pragma once

#include "ollamaconfig.hpp"

namespace DataObjects {

/**
 * @brief The OllamaQwenConfig class Configuration for Qwen3.5 mainly, but can be used for other Qwen models also
 */
class OllamaQwenConfig : public OllamaConfig
{
public:
    OllamaQwenConfig();

    void enableThinkingMode(bool enable);
};

} // namespace DataObjects
