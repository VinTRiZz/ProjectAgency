#include "objectsetting.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

namespace Exchange {

std::string ObjectSetting::toJson() const
{
    nlohmann::json res;

    res["name"] = Encryption::encodeHex(m_name);
    res["value"] = Encryption::encodeHex(m_value);

    m_error.reset();
    return res.dump();
}

bool ObjectSetting::readJson(const std::string_view &iString)
{
    try {
        auto settingJson = nlohmann::json::parse(iString);
        if (!settingJson.contains("name") || !settingJson.contains("value")) {
            m_error.setCode(ErrorCode::ProtocolJsonException);
            m_error.setDetailText("ObjectSetting | Invalid format");
            return false;
        }

        m_name = Encryption::decodeHex(settingJson["name"]);
        m_value = Encryption::decodeHex(settingJson["value"]);

        m_error.reset();
    } catch (nlohmann::json::exception& ex) {
        m_error.setCode(ErrorCode::ProtocolJsonException);
        m_error.setDetailText(std::string("ObjectSetting | ") + ex.what());
        return false;
    }
    return true;
}



}