#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Common/ApplicationSettings.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <regex>

#include <thread>

#include "business/aibackend.hpp"
#include "business/settings.hpp"

namespace bpo = boost::program_options;

// TODO: Think, how to do this thing properly (don't like such format)
#define APP_EXITCODE_OK                   0
#define APP_EXITCODE_CONFIGURATION_ERROR  1
#define APP_EXITCODE_FAILURE              2
#define APP_EXITCODE_EXCEPTION            3
#define APP_EXITCODE_UNKNOWN_EXCEPTION    4


int main(int argc, char* argv[]) {

    // Common setings
    uint16_t    wsControlPort {0};
    std::string dataDir {"."};

    bpo::options_description desc;
    desc.add_options()
            ("help",                                        "Print help and exit")
            ("control,-c",      bpo::value(&wsControlPort), "Manager control port")
            ("data,-d",         bpo::value(&dataDir),       "Path to directory to use for saving data (current dir by default)")
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
    dataDir = std::filesystem::path(dataDir) / "PAG-AIBackend";

    // Setup root of application and logging
    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath(dataDir);
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

    // Load settings
    auto& settingsInstance = Common::ApplicationSettings::getInstance();
    auto settingsFile = dirManager.getDirectory(Common::DirectoryManager::Config) / "aibackend.ini";
    settingsInstance.loadSettings(settingsFile);

    // Add expected settings
    if (!settingsInstance.hasSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN)) {
        settingsInstance.addSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN);
    }
    if (!settingsInstance.hasSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_OLLAMA_SERVER)) {
        settingsInstance.addSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_OLLAMA_SERVER);
    }
    if (!settingsInstance.hasSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_OLLAMA_PORT)) {
        settingsInstance.addSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_OLLAMA_PORT);
    }
    if (!settingsInstance.hasSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_CONTROL_PORT)) {
        settingsInstance.addSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_CONTROL_PORT);
    }
    settingsInstance.saveSettings(); // For section value adding

    // Check control port
    auto pPortSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_CONTROL_PORT);
    if (wsControlPort == 0) {
        if (!pPortSetting->getValue().has_value()) {
            COMPLOG_ERROR("Invalid manager control port. Set it in settings file or arguments of application");
            return APP_EXITCODE_CONFIGURATION_ERROR;
        }

        try {
            wsControlPort = std::get<long long>(pPortSetting->getValue().value());
            if (wsControlPort < 0 || wsControlPort > 65535) {
                throw std::invalid_argument("Port value exception");
            }
        } catch (std::bad_variant_access& ex) {
            COMPLOG_ERROR("Invalid port value. Acceptable value - integer, from 0 to 65535");
            return APP_EXITCODE_CONFIGURATION_ERROR;
        } catch (std::invalid_argument& ex) {
            COMPLOG_ERROR("Invalid port value. Acceptable value - integer, from 0 to 65535");
            return APP_EXITCODE_CONFIGURATION_ERROR;
        }
    }

    // Check Ollama server settings
    auto pOllamaPortSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_OLLAMA_PORT);
    auto pOllamaAddressSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_OLLAMA_SERVER);
    if (!pOllamaPortSetting->getValue().has_value() || !pOllamaAddressSetting->getValue().has_value()) {
        COMPLOG_ERROR("Ollama server not set. Configure it in configuration file");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    // Check Ollama server IP
    const std::regex pattern(
        R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"
        );
    if (!std::regex_match(pOllamaAddressSetting->getValueString(), pattern)) {
        COMPLOG_ERROR("Invalid ollama server address. Acceptable: IPv4 address");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    // Check Ollama server port
    long long ollamaAPIport = 0;
    try {
        ollamaAPIport = std::get<long long>(pOllamaPortSetting->getValue().value());
        if (ollamaAPIport < 0 || ollamaAPIport > 65535) {
            throw std::invalid_argument("Port value exception");
        }
    } catch (std::bad_variant_access& ex) {
        COMPLOG_ERROR("Invalid ollama server port value. Acceptable value - integer, from 0 to 65535");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    } catch (std::invalid_argument& ex) {
        COMPLOG_ERROR("Invalid ollama server port value. Acceptable value - integer, from 0 to 65535");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    AIBackend backend;
    auto tokenSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN);

    try {
    backend.start(tokenSetting->getValueString(),
                  wsControlPort,
                  pOllamaAddressSetting->getValueString(),
                  ollamaAPIport);
    } catch (const std::exception& ex) {
        try {
            backend.stop();
        } catch (...) {
            std::cerr << "CRITICAL: FAILED TO STOP APP AFTER FAILURE" << std::endl;
            return APP_EXITCODE_FAILURE;
        }
        std::cerr << "CRITICAL: EXCEPTION: " << ex.what() << std::endl;
        return APP_EXITCODE_EXCEPTION;
    } catch (...) {
        try {
            backend.stop();
        } catch (...) {
            std::cerr << "CRITICAL: FAILED TO STOP APP AFTER FAILURE" << std::endl;
            return APP_EXITCODE_FAILURE;
        }
        std::cerr << "CRITICAL: UNKNOWN EXCEPTION" << std::endl;
        return APP_EXITCODE_UNKNOWN_EXCEPTION;
    }
    return APP_EXITCODE_OK;
}
