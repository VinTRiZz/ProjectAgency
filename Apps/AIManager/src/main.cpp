#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Ecosystem/Utility.h>

#include <ProjectAgency/Exchange/Error.h>

#include <boost/program_options.hpp>

#include <iostream>

#include "business/applicationcore.hpp"
#include "common/settingsconfigurator.hpp"
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
    Common::setupBacktrace();

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
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::Logs));

    // Load settings
    auto& settingsInstance = Common::ApplicationSettings::getInstance();
    auto settingsFile = dirManager.getDirectory(Common::Config) / "aimanager.ini";
    settingsInstance.loadSettings(settingsFile);
    AIManagerCommon::SettingsConfigurator settingsConfigurator;
    try {
        settingsConfigurator.setupSettings();
    } catch (const std::exception& ex) {
        COMPLOG_ERROR("Invalid setting value:", ex.what());
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    // Check API port
    if (httpAPIPort == 0) {
        try {
            auto pApiPortSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_API_PORT);
            if (!pApiPortSetting->isSet()) {
                COMPLOG_ERROR("Invalid API port value (not set). Acceptable value - integer, from 0 to 65535");
                return APP_EXITCODE_CONFIGURATION_ERROR;
            }
            httpAPIPort = pApiPortSetting->getValue<int64_t>();
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
    if (!tokenSetting->isSet()) {
        COMPLOG_ERROR("Manager token not set");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }
    auto token = tokenSetting->getValueString();

    ApplicationCore app;
    app.setToken(token);
    if (!app.init()) {
        COMPLOG_ERROR("Failed to start application (init failed)");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    try {
        app.start(httpAPIPort);
    } catch (const Exchange::Error& ex) {
        ex.printSelfStd();
        Common::printStacktraceNoLogger();
        try {
            app.stop();
        } catch (...) {
            throw;
        }
        return APP_EXITCODE_EXCEPTION;
    } catch (const std::exception& ex) {
        std::cout << std::endl << ex.what() << std::endl;
        Common::printStacktraceNoLogger();
        try {
            app.stop();
        } catch (...) {
            throw;
        }
        return APP_EXITCODE_EXCEPTION;
    } catch (...) {
        std::cout << "UNKNOWN EXCEPTION" << std::endl;
        Common::printStacktraceNoLogger();
        try {
            app.stop();
        } catch (...) {
            throw;
        }
        return APP_EXITCODE_UNKNOWN_EXCEPTION;
    }
    return APP_EXITCODE_OK;
}
