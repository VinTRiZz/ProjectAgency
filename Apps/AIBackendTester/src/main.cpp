#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Ecosystem/Utility.h>

#include <ProjectAgency/Exchange/Error.h>

#include <boost/program_options.hpp>

#include <boost/stacktrace.hpp>

#include "tester/aibackendtester.hpp"

namespace bpo = boost::program_options;

#define APP_EXITCODE_OK                   0
#define APP_EXITCODE_CONFIGURATION_ERROR  1
#define APP_EXITCODE_EXCEPTION            3
#define APP_EXITCODE_UNKNOWN_EXCEPTION    4

int main(int argc, char* argv[])
{
    std::string dataDir {"."};
    Common::setupBacktrace();

    bpo::options_description desc;
    desc.add_options()
        ("help",                                          "Print help and exit")
        ("data,-d",   bpo::value(&dataDir),               "Path to directory to use for saving data (current dir by default)")
        ;

    bpo::variables_map vm;

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

    if (!std::filesystem::exists(dataDir)) {
        std::cerr << "Invalid data directory path: " << dataDir << std::endl;
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }
    dataDir = std::filesystem::path(dataDir) / "PAG-AIBackendTester";

    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath(dataDir);
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::Logs));

    try {
        AIBackendTester tester;
        tester.run();
    } catch (const Exchange::Error& ex) {
        ex.printSelfStd();
        Common::printStacktraceNoLogger();
        return APP_EXITCODE_EXCEPTION;
    } catch (const std::exception& ex) {
        std::cout << std::endl << ex.what() << std::endl;
        Common::printStacktraceNoLogger();
        return APP_EXITCODE_EXCEPTION;
    } catch (...) {
        std::cout << "UNKNOWN EXCEPTION" << std::endl;
        Common::printStacktraceNoLogger();
        return APP_EXITCODE_UNKNOWN_EXCEPTION;
    }

    return APP_EXITCODE_OK;
}