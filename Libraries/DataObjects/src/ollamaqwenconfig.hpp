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

    struct Specific
    {
        bool enableThinking {true};
    };
    Specific m_specific;

    virtual std::string toJson() const override;
    virtual bool readJson(const std::string_view& iString) override;
};

} // namespace DataObjects
