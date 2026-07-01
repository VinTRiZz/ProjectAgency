#pragma once

#include "ollamaconfig.hpp"

namespace AIObjects {

/**
 * @brief The OllamaDeepSeekConfig class Configuration for DeepSeek-R1 mainly, but can be used for DeepSeek line also
 */
class OllamaDeepSeekConfig : public OllamaConfig {
public:
    OllamaDeepSeekConfig();

    struct Specific
    {
        bool enableThinking {true};
    };
    Specific m_specific;

    virtual std::string toJson() const override;
    virtual bool readJson(const std::string_view& iString) override;
};

} // namespace AIObjects
