#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Ecosystem/Utility.h>

#include <ProjectAgency/Exchange/Error.h>

#include <boost/program_options.hpp>

#include <boost/stacktrace.hpp>

#include "business/aibackend.hpp"
#include "business/settings.hpp"

#include <signal.h>

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
    Common::setupBacktrace();

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
    } catch (const bpo::error& er) {
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
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::Logs));

    // Load settings
    auto& settingsInstance = Common::ApplicationSettings::getInstance();
    auto settingsFile = dirManager.getDirectory(Common::Config) / "aibackend.ini";
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
        if (!pPortSetting->isSet()) {
            Exchange::Error::printSelf(
                Exchange::ErrorCode::SystemInvalidConfig,
                std::string("Manager control port is invalid"));
            return APP_EXITCODE_CONFIGURATION_ERROR;
        }

        try {
            wsControlPort = pPortSetting->getValue<int64_t>();
            if (wsControlPort < 0 || wsControlPort > 65535) {
                throw std::invalid_argument("Port value exception");
            }
        } catch (const std::exception& ex) {
            Exchange::Error::printSelf(
                Exchange::ErrorCode::SystemInvalidConfig,
                std::string("Invalid WS control port"));
            return APP_EXITCODE_CONFIGURATION_ERROR;
        }
    }

    // Check Ollama server settings
    auto pOllamaPortSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_OLLAMA_PORT);
    auto pOllamaAddressSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_OLLAMA_SERVER);
    if (!pOllamaPortSetting->isSet() || !pOllamaAddressSetting->isSet()) {
        Exchange::Error::printSelf(
            Exchange::ErrorCode::SystemInvalidConfig,
            std::string("Ollama server not set"));
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    // Check Ollama server port
    long long ollamaAPIport = 0;
    try {
        ollamaAPIport = pOllamaPortSetting->getValue<int64_t>();
        if (ollamaAPIport < 0 || ollamaAPIport > 65535) {
            throw std::invalid_argument("Port value exception");
        }
    } catch (const std::exception& ex) {
        Exchange::Error::printSelf(
            Exchange::ErrorCode::SystemInvalidConfig,
            std::string("Ollama port is invalid"));
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    AIBackend backend;
    auto tokenSetting = settingsInstance.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_MANAGER_TOKEN);

    try {
        backend.start(tokenSetting->getValueString(),
                      wsControlPort,
                      pOllamaAddressSetting->getValueString(),
                      ollamaAPIport);
    } catch (const Exchange::Error& ex) {
        ex.printSelfStd();
        Common::printStacktraceNoLogger();
        try {
            backend.stop();
        } catch (...) {
            throw;
        }
        return APP_EXITCODE_EXCEPTION;
    } catch (const std::exception& ex) {
        std::cout << std::endl << ex.what() << std::endl;
        Common::printStacktraceNoLogger();
        try {
            backend.stop();
        } catch (...) {
            throw;
        }
        return APP_EXITCODE_EXCEPTION;
    } catch (...) {
        std::cout << "UNKNOWN EXCEPTION" << std::endl;
        Common::printStacktraceNoLogger();
        try {
            backend.stop();
        } catch (...) {
            throw;
        }
        return APP_EXITCODE_UNKNOWN_EXCEPTION;
    }
    return APP_EXITCODE_OK;
}
