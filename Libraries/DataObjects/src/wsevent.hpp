#pragma once

#include "serializableobject.hpp"

namespace DataObjects::Events {

/**
 * @brief The EventType enum Type of WS event
 */
enum EventType : int
{
    // 0..99 - etc events
    EtcUnknown = 0,
    EtcResultSuccess,
    EtcResultMessage,

    // 200..299 - Exchange events
    ExchangeUnknown = 200,

    // 300..399 - AI events
    AIStart = 300,
    AIStatus,
    AIStop,
    AIAsk,
    AIAskStatus,
    AISetCommonSettings,
};

/**
 * @brief The WSEvent class Event store class for handling events
 */
class WSEvent : public SerializableObject
{
public:
    WSEvent(EventType etype = EventType::EtcUnknown);

    bool isValid() const;

    EventType getType() const;

    void setPayload(const std::string& payload);
    std::string_view getPayload() const;

    // SerializableObject interface
    std::string toJson() const override;
    bool readJson(const std::string &iString) override;

private:
    EventType   m_type {EventType::EtcUnknown};
    std::string m_payload;
};

} // namespace Events
