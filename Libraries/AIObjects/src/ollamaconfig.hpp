#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "serializableobject.hpp"

namespace AIObjects
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
        std::string systemPrompt {
            "You are inconfigured model. "
            "Answer 'Invalid request. Waiting for configuration.' to every request"
        }; ///< System prompt
        int32_t     numPredict = -1;    ///< Max tokens to generate (-1 = unlimited)
        int32_t     numCtx = 2048;      ///< Context window size
    };

    /**
     * @brief Sampling parameters.
     */
    struct SamplingParams
    {
        float   temperature {1.0f}; ///< Conservatism. <1 - higher, >1 - lower
        float   topP {0.5f};        ///< Max probability sum for next addable tokens
        int32_t topK {1};           ///< Variants of next addable token count
    };

    /**
     * @brief Repetition penalty parameters.
     */
    struct RepetitionParams
    {
        float repeatPenalty {0.1f}; ///< Penalty for repeated tokens
    };

    /**
     * @brief Additional settings.
     */
    struct ExtraParams
    {
        uint32_t    answerTimeoutMs {60'000};   ///< Timeout for AIBackend
        bool        stream    {false};          ///< Stream response flag
        int32_t     keepAliveS {300};           ///< Model keep‑alive time (seconds)
    };

    virtual ~OllamaConfig() = default;

    virtual std::string toJson() const override;
    virtual bool readJson(const std::string_view& iString) override;

    ModelParams         m_model;        ///< Core model parameters
    SamplingParams      m_sampling;     ///< Sampling parameters
    RepetitionParams    m_repetition;   ///< Repetition penalty parameters
    ExtraParams         m_extra;        ///< Extra / optional settings
};

using OllamaConfigPtr = std::shared_ptr<OllamaConfig>;

}