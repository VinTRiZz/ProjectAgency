#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "serializableobject.hpp"

namespace DataObjects
{

/**
 * @brief The OllamaConfig class Configuration of model in Ollama
 */
class OllamaConfig : public SerializableObject
{
public:
    /**
     * @brief Core model parameters (API-related).
     */
    struct ModelParams
    {
        std::string name;               ///< Model name
        std::string systemPrompt;       ///< System prompt
        int32_t     numPredict = -1;    ///< Max tokens to generate (-1 = unlimited)
        int32_t     numCtx = 4096;      ///< Context window size
    };

    /**
     * @brief Sampling parameters.
     */
    struct SamplingParams
    {
        float temperature = 0.05f;       ///< Temperature (higher = more random)
        float topP = 0.1f;               ///< Word expectance. Lower - expected
        int32_t topK = 10;               ///< Max variant count
    };

    /**
     * @brief Repetition penalty parameters.
     */
    struct RepetitionParams
    {
        float repeatPenalty = 1.1f; ///< Penalty for repeated tokens
    };

    /**
     * @brief Additional settings.
     */
    struct ExtraParams
    {
        bool        stream    {false};  ///< Stream response flag
        int32_t     keepAlive {300};    ///< Model keep‑alive time (seconds)
        std::string prompt;             ///< What goes before generation result
        std::string suffix;             ///< What goes after generation result
    };

    virtual ~OllamaConfig() = 0;

    virtual std::string toJson() const override;
    virtual bool readJson(const std::string_view& iString) override;

    ModelParams m_model;           ///< Core model parameters
    SamplingParams m_sampling;     ///< Sampling parameters
    RepetitionParams m_repetition; ///< Repetition penalty parameters
    ExtraParams m_extra;           ///< Extra / optional settings
};

using OllamaConfigPtr = std::shared_ptr<OllamaConfig>;

}