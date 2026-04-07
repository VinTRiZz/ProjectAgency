#pragma once

#include "serializableobject.hpp"

namespace DataObjects {

class AIRequest : public SerializableObject
{
public:

    // SerializableObject interface
    std::string toJson() const override;
    [[deprecated("Requiest must not be converted from JSON back")]] bool readJson(const std::string &iString) override;

    void setRequest(const std::string& request);
    void setModel(const std::string& modelName);
    void setStreamingEnabled(bool isEn);

private:
    std::string m_request;
    std::string m_modelName;
    bool        m_isStreamingEnabled {false};
};

} // namespace DataObjects
