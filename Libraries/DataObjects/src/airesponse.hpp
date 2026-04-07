#pragma once

#include "serializableobject.hpp"

namespace DataObjects {

class AIResponse : public SerializableObject
{
public:

    // SerializableObject interface
    [[deprecated("Response must not be converted back")]] std::string toJson() const override;
    bool readJson(const std::string &iString) override;

    std::string getModelName() const;
    std::string getResponse() const;
    std::string getTimestamp() const;
    std::string getDoneReason() const;
    std::string getContext() const;

private:
    std::string m_modelName;
    std::string m_response;
    std::string m_timestamp;
    std::string m_doneReason;
    std::string m_context;
};

} // namespace DataObjects