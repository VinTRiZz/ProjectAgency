#include "wsevent.hpp"

#include <nlohmann/json.hpp>
#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

namespace DataObjects::Events {

WSEvent::WSEvent(EventType etype) :
    m_type {etype}
{

}

bool WSEvent::isValid() const
{
    return (m_type != EventType::EtcUnknown);
}

void WSEvent::setType(EventType etype)
{
    m_type = etype;
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
        return true;
    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_ERROR("[WSEvent] Parsing error:", ex.what());
    }
    return false;
}

} // namespace Events
