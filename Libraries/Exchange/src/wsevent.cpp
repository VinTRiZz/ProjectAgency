#include "wsevent.hpp"

#include <nlohmann/json.hpp>
#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

namespace Exchange::Events {

WSEvent::WSEvent(EventType etype) :
    m_type {etype}
{

}

void WSEvent::setId(uint64_t eventId)
{
    m_eventId = eventId;
    if (m_eventId == 0) {
        m_error.setCode(ErrorCode::ProtocolWSInvalidEventId);
    } else {
        m_error.setCode(ErrorCode::NoError);
    }
}

uint64_t WSEvent::getId() const
{
    return m_eventId;
}

void WSEvent::setType(EventType etype)
{
    m_type = etype;
    if (m_type == EventType::EtcUnknown) {
        m_error.setCode(ErrorCode::ProtocolWSInvalidEventType);
    } else {
        m_error.setCode(ErrorCode::NoError);
    }
}

EventType WSEvent::getType() const
{
    return m_type;
}

void WSEvent::setPayload(const std::string &payload)
{
    m_payload = payload;
}

std::string_view WSEvent::getPayload() const
{
    return m_payload;
}

std::string WSEvent::toJson() const
{
    nlohmann::json res;
    res["type"] = m_type;
    res["payload"] = Encryption::encodeHex(m_payload);
    return res.dump();
}

bool WSEvent::readJson(const std::string_view &iString)
{
    m_type = EventType::EtcUnknown;
    try {
        auto parsedJson = nlohmann::json::parse(iString);
        m_type      = parsedJson["type"];
        m_payload   = Encryption::decodeHex(parsedJson["payload"]);
        m_error.setCode(ErrorCode::NoError);
        return true;
    } catch (const nlohmann::json::exception& ex) {
        m_error.setCode(ErrorCode::ProtocolJsonException);
        m_error.setDetailText(ex.what());
    }
    return false;
}

} // namespace Events
