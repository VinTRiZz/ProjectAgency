#include "devicestatus.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>

namespace Exchange {

std::string DeviceStatus::toJson() const
{
    nlohmann::json res;
    res["common"]["uptime"] = common.uptime;

    res["cpu"]["temp"] = cpu.temperature;
    res["cpu"]["load"] = cpu.loadPercent;

    res["storage"]["space_total"]       = storage.spaceTotal;
    res["storage"]["space_available"]   = storage.spaceAvailable;
    res["storage"]["space_free"]        = storage.spaceFree;

    m_error.setCode(ErrorCode::NoError);
    return res.dump();
}

bool DeviceStatus::readJson(const std::string_view &iString)
{
    try {
        auto statusJson = nlohmann::json::parse(iString);

        common.uptime = statusJson["common"]["uptime"];

        cpu.loadPercent = statusJson["cpu"]["load"];
        cpu.temperature = statusJson["cpu"]["temp"];

        // Funny, that "uintmax_t" in nlohmann::json is double in Qt
        storage.spaceTotal       = statusJson["storage"]["space_total"];
        storage.spaceAvailable   = statusJson["storage"]["space_available"];
        storage.spaceFree        = statusJson["storage"]["space_free"];

        m_error.setCode(ErrorCode::NoError);
    } catch (nlohmann::json::exception& ex) {
        m_error.setCode(ErrorCode::ProtocolJsonException);
        m_error.setDetailText(std::string("DeviceStatus | ") + ex.what());
        return false;
    }
    return true;
}

} // namespace AIObjects
