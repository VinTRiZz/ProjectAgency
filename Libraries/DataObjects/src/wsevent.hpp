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
    AIOllamaStart = 300,
    AIOllamaStatus,
    AIOllamaStop,
    AIAsk,
    AIAskStatus,
    AIAskInterrupt,
    AIAskSetConfig,
};

/**
 * @brief The WSEvent class Event store class for handling events
 */
class WSEvent : public SerializableObject
{
public:
    WSEvent(EventType etype = EventType::EtcUnknown);

    bool isValid() const;

    // ID = 0 --> invalid, so it's required to set ID (even as random number)
    void setId(uint64_t eventId);
    uint64_t getId() const;

    void setType(EventType etype);
    EventType getType() const;

    void setPayload(const std::string& payload);
    std::string_view getPayload() const;

    // SerializableObject interface
    std::string toJson() const override;
    bool readJson(const std::string_view &iString) override;

private:
    uint64_t m_eventId {0};
    EventType   m_type {EventType::EtcUnknown};
    std::string m_payload;
};

} // namespace Events
