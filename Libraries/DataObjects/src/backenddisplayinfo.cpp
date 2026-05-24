#include "backenddisplayinfo.hpp"

#include <nlohmann/json.hpp>
#include <Components/Encryption/Encoding.h>
#include <Components/Logger/Logger.h>

namespace DataObjects {

std::string BackendDisplayInfo::toJson() const
{
    nlohmann::json res;

    // TODO: Add fields (now not required)
    res["is_online"] = isOnline;
    res["name"]      = Encryption::encodeHex(name);

    return res.dump();
}

bool BackendDisplayInfo::readJson(const std::string_view &iString)
{
    try {
        auto statusJson = nlohmann::json::parse(iString);

        isOnline    = statusJson["is_online"];
        name        = Encryption::decodeHex(statusJson["name"]);

        // TODO: Add fields (now not required)

    } catch (nlohmann::json::exception& ex) {
        COMPLOG_ERROR("BackendDisplayInfo Parse error:", ex.what());
        return false;
    }
    return true;
}

}