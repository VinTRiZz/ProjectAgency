#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

#include <boost/program_options.hpp>

#include <filesystem>
#include <iostream>

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {

    // Common setings
    uint16_t    httpAPIPort {9002};
    std::string dataDir {"."};

    bpo::options_description desc;
    desc.add_options()
            ("api-port,-a", bpo::value(&httpAPIPort),   "HTTP API port for control window")
            ("data,-d",     bpo::value(&dataDir),       "Path to directory to use for saving server data (current dir by default)")
            ;

    // Harvest settings
    try {
        auto options = bpo::parse_command_line(argc, argv, desc, bpo::command_line_style::unix_style);
        bpo::variables_map vm;
        bpo::store(options, vm);
        bpo::notify(vm);
    } catch (bpo::error& er) {
        std::cerr << er.what() << std::endl;
        desc.print(std::cerr);
        std::cerr << std::endl;
        return 1;
    }

    // Check-up
    if (!std::filesystem::exists(dataDir)) {
        std::cerr << "Invalid data directory path: " << dataDir << std::endl;
        return 1;
    }
    dataDir = std::filesystem::path(dataDir) / "PAG-AIManager";

    // Setup root of application and logging
    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath(dataDir);
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

    return 0;
}
