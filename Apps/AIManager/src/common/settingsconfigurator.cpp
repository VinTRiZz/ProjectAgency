#include "settingsconfigurator.hpp"

#include <Components/Ecosystem/ApplicationSettings.h>

#include "settings.hpp"

namespace AIManagerCommon
{

void SettingsConfigurator::setupSettings()
{
    auto& settingsInstance = Common::ApplicationSettings::getInstance();

    auto initSetting = [&settingsInstance](const auto& sect, const auto& name){
        if (!settingsInstance.hasSetting(sect, name)) {
            settingsInstance.addSetting(sect, name);
        }
    };

    auto initSettingValue = [&settingsInstance](const auto& sect, const auto& name, auto&& value){
        if (!settingsInstance.hasSetting(sect, name)) {
            settingsInstance.addSetting(sect, name);
        }
        auto pSetting = settingsInstance.getSetting(sect, name);
        if (!pSetting->isSet()) {
            pSetting->setValue(value);
        }
    };

    // App common
    initSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN);
    initSettingValue(Settings::SECTION_SYSTEM, Settings::SYSTEM_API_PORT, 9001);
    initSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_INPUT_MODEL);

    // For debug needs, will not work in real cases
    initSettingValue(Settings::SECTION_DB, Settings::SYSTEM_MANAGER_TOKEN,  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    // DB
    initSettingValue(Settings::SECTION_DB, Settings::DB_ADDRESS,    "127.0.0.1");
    initSettingValue(Settings::SECTION_DB, Settings::DB_PORT,       10001);
    initSettingValue(Settings::SECTION_DB, Settings::DB_DBNAME,     "pag_main");
    initSettingValue(Settings::SECTION_DB, Settings::DB_USERNAME,   "server");
    initSettingValue(Settings::SECTION_DB, Settings::DB_USER_PASS,  "serv_auth_password");

    // Check DB Values
    auto portValue = settingsInstance.getSetting(Settings::SECTION_DB, Settings::DB_PORT)->getValue<int64_t>();
    if (0 > portValue || 65535 < portValue) {
        throw std::invalid_argument(std::string("database port (") + std::to_string(portValue) + ")");
    }

    settingsInstance.saveSettings();
}

}