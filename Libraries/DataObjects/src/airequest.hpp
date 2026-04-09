#pragma once

#include "serializableobject.hpp"

#include <optional>
#include <variant>
#include <vector>

namespace DataObjects {

class AIRequest : public SerializableObject {
public:
    const std::string& model() const;
    void setModel(const std::string& value);

    const std::string& prompt() const;
    void setRequest(const std::string& request);

    const std::optional<std::string>& suffix() const;
    void setSuffix(const std::optional<std::string>& value);

    const std::vector<std::string>& images() const;
    void setImages(const std::vector<std::string>& value);

    const std::optional<std::string>& format() const;
    void setFormat(const std::optional<std::string>& value);

    const std::optional<std::string>& system() const;
    void setSystem(const std::optional<std::string>& value);

    const std::optional<std::string>& templateStr() const;
    void setTemplateStr(const std::optional<std::string>& value);

    const std::optional<std::vector<int>>& context() const;
    void setContext(const std::optional<std::vector<int>>& value);

    std::optional<bool> stream() const;
    void setStream(std::optional<bool> value);

    std::optional<bool> raw() const;
    void setRaw(std::optional<bool> value);

    const std::optional<std::variant<int, std::string>>& keepAlive() const;
    void setKeepAlive(const std::optional<std::variant<int, std::string>>& value);

    std::optional<int> numPredict() const;
    void setNumPredict(std::optional<int> value);

    std::optional<float> temperature() const;
    void setTemperature(std::optional<float> value);

    std::optional<int> topK() const;
    void setTopK(std::optional<int> value);

    std::optional<float> topP() const;
    void setTopP(std::optional<float> value);

    std::optional<int64_t> seed() const;
    void setSeed(std::optional<int64_t> value);

    const std::optional<std::vector<std::string>>& stop() const;
    void setStop(const std::optional<std::vector<std::string>>& value);

    std::optional<int> numCtx() const;
    void setNumCtx(std::optional<int> value);

    std::optional<float> repeatPenalty() const;
    void setRepeatPenalty(std::optional<float> value);

    std::optional<int> repeatLastN() const;
    void setRepeatLastN(std::optional<int> value);

    std::optional<int> numGpu() const;
    void setNumGpu(std::optional<int> value);

    std::optional<int> numThread() const;
    void setNumThread(std::optional<int> value);

    // SerializableObject interface
    std::string toJson() const override;
    [[deprecated("Request must not be converted from JSON back")]]
    bool readJson(const std::string& iString) override;

private:
    std::string m_model;
    std::string m_prompt;
    std::optional<std::string>  m_suffix;
    std::vector<std::string>    m_images;
    std::optional<std::string>  m_format;
    std::optional<std::string>  m_system;
    std::optional<std::string>  m_template;
    std::optional<std::vector<int>> m_context;
    std::optional<bool> m_stream;
    std::optional<bool> m_raw;
    std::optional<std::variant<int, std::string>> m_keepAlive;

    std::optional<int>      m_numPredict;
    std::optional<float>    m_temperature;
    std::optional<int>      m_topK;
    std::optional<float>    m_topP;
    std::optional<int64_t>  m_seed;
    std::optional<std::vector<std::string>> m_stop;
    std::optional<int>      m_numCtx;
    std::optional<float>    m_repeatPenalty;
    std::optional<int>      m_repeatLastN;
    std::optional<int>      m_numGpu;
    std::optional<int>      m_numThread;
};

} // namespace DataObjects
