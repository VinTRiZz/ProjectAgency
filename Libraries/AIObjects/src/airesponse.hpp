#pragma once

#include "serializableobject.hpp"

#include <stdint.h>
#include <vector>

namespace AIObjects {

class AIResponse : public SerializableObject
{
public:

    // SerializableObject interface
    std::string toJson() const override;
    bool readJson(const std::string_view &iString) override;

    std::string getModelName() const;
    std::string getThinking() const;
    std::string getResponse() const;
    std::string getTimestamp() const;
    std::string getDoneReason() const;
    std::vector<uint32_t> getContext() const;

private:
    std::string m_modelName;
    std::string m_thinking;
    std::string m_response;
    std::string m_timestamp;
    std::string m_doneReason;
    std::vector<uint32_t> m_context;
    bool        m_isDone {false};
};

} // namespace AIObjects