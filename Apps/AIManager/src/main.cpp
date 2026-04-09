#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Common/ApplicationSettings.h>

#include <boost/program_options.hpp>

#include <iostream>

#include "business/aimanager.hpp"
#include "common/settings.hpp"

namespace bpo = boost::program_options;

// TODO: Think, how to do this thing properly (don't like such format)
#define APP_EXITCODE_OK                   0
#define APP_EXITCODE_CONFIGURATION_ERROR  1
#define APP_EXITCODE_FAILURE              2
#define APP_EXITCODE_EXCEPTION            3
#define APP_EXITCODE_UNKNOWN_EXCEPTION    4


int main(int argc, char* argv[]) {

    // Common setings
    long long   httpAPIPort {0};
    std::string dataDir {"."};

    bpo::options_description desc;
    desc.add_options()
            ("help",                                    "Print help and exit")
            ("api-port,-a", bpo::value(&httpAPIPort),   "HTTP API port for control window")
            ("data,-d",     bpo::value(&dataDir),       "Path to directory to use for saving server data (current dir by default)")
            ;
    bpo::variables_map vm;

    // Harvest settings
    try {
        auto options = bpo::parse_command_line(argc, argv, desc, bpo::command_line_style::unix_style);
        bpo::store(options, vm);
        bpo::notify(vm);
    } catch (bpo::error& er) {
        std::cerr << er.what() << std::endl;
        desc.print(std::cerr);
        std::cerr << std::endl;
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return APP_EXITCODE_OK;
    }

    // Check-up
    if (!std::filesystem::exists(dataDir)) {
        std::cerr << "Invalid data directory path: " << dataDir << std::endl;
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }
    dataDir = std::filesystem::path(dataDir) / "PAG-AIManager";

    // Setup root of application and logging
    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath(dataDir);
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

    // Load settings
    auto& settingsInstance = Common::ApplicationSettings::getInstance();
    auto settingsFile = dirManager.getDirectory(Common::DirectoryManager::Config) / "aimanager.ini";
    settingsInstance.loadSettings(settingsFile);

    // Add expected settings
    if (!settingsInstance.hasSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN)) {
        settingsInstance.addSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN);
    }
    if (!settingsInstance.hasSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_API_PORT)) {
        settingsInstance.addSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_API_PORT);
    }
    settingsInstance.saveSettings();

    // Check API port
    if (httpAPIPort == 0) {
        try {
            auto pApiPortSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_API_PORT);
            if (!pApiPortSetting->getValue().has_value()) {
                COMPLOG_ERROR("Invalid API port value (not set). Acceptable value - integer, from 0 to 65535");
                return APP_EXITCODE_CONFIGURATION_ERROR;
            }
            httpAPIPort = std::get<long long>(pApiPortSetting->getValue().value());
        } catch (std::bad_variant_access& ex) {
            COMPLOG_ERROR("Invalid API port value. Acceptable value - integer, from 0 to 65535");
            return APP_EXITCODE_CONFIGURATION_ERROR;
        }
    }

    if (httpAPIPort < 0 || httpAPIPort > 65535) {
        COMPLOG_ERROR("Invalid API port value. Acceptable value - integer, from 0 to 65535");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    // Token
    auto tokenSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN);
    if (!tokenSetting->getValue().has_value()) {
        COMPLOG_ERROR("Manager token not set");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }
    auto token = tokenSetting->getValueString();

    AIManager manager;
    manager.setToken(token);
    manager.initBackends();

    try {
        manager.start(httpAPIPort);
    } catch (const std::exception& ex) {
        try {
            manager.stop();
        } catch (...) {
            std::cerr << "CRITICAL: FAILED TO STOP APP AFTER FAILURE" << std::endl;
            return APP_EXITCODE_FAILURE;
        }
        std::cerr << "CRITICAL: EXCEPTION: " << ex.what() << std::endl;
        return APP_EXITCODE_EXCEPTION;
    } catch (...) {
        try {
            manager.stop();
        } catch (...) {
            std::cerr << "CRITICAL: FAILED TO STOP APP AFTER FAILURE" << std::endl;
            return APP_EXITCODE_FAILURE;
        }
        std::cerr << "CRITICAL: UNKNOWN EXCEPTION" << std::endl;
        return APP_EXITCODE_UNKNOWN_EXCEPTION;
    }
    return APP_EXITCODE_OK;
}
