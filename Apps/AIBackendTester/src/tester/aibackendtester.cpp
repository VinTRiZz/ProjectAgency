#include "aibackendtester.hpp"

#include "actions/actionconnect.hpp"
#include "actions/actiondisconnect.hpp"
#include "actions/actionping.hpp"
#include "actions/actionask.hpp"
#include "actions/actionaskstatus.hpp"
#include "actions/actionaskinterrupt.hpp"
#include "actions/actionsetconfig.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/Utility.h>

struct AIBackendTester::Impl
{
    WebSockets::Client client;
    std::map<std::string, std::unique_ptr<TesterAction>> actions;
    bool running {true};
};

AIBackendTester::AIBackendTester() :
    d {new Impl}
{
    registerActions();
}

AIBackendTester::~AIBackendTester() = default;

void AIBackendTester::registerActions()
{
    d->actions["connect"]       = std::make_unique<ConnectAction>();
    d->actions["disconnect"]    = std::make_unique<DisconnectAction>();
    d->actions["ping"]          = std::make_unique<PingAction>();
    d->actions["ask"]           = std::make_unique<AskAction>();
    d->actions["ask_status"]    = std::make_unique<AskStatusAction>();
    d->actions["ask_interrupt"] = std::make_unique<AskInterruptAction>();
    d->actions["set_config"]    = std::make_unique<SetConfigAction>();
}

void AIBackendTester::printHelp() const
{
    COMPLOG_EMPTY("==============================================");
    COMPLOG_EMPTY("  AIBackendTester - Supported Commands:");
    COMPLOG_EMPTY("----------------------------------------------");
    COMPLOG_EMPTY("  help                           Show this help");
    COMPLOG_EMPTY("  exit                           Exit the tester");

    for (const auto& [name, action] : d->actions) {
        COMPLOG_EMPTY(" ", action->getUsage());
        COMPLOG_EMPTY("      ", action->getDescription());
    }
    COMPLOG_EMPTY("----------------------------------------------");
    COMPLOG_EMPTY("  Arguments in quotes: ask \"your prompt here\"");
    COMPLOG_EMPTY("==============================================");
}

std::vector<std::string> AIBackendTester::parseArguments(const std::string& line) const
{
    std::vector<std::string> args;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        args.push_back(current);
    }
    return args;
}

bool AIBackendTester::processLine(const std::string& line)
{
    if (line.empty()) {
        return true;
    }

    auto args = parseArguments(line);
    if (args.empty()) {
        return true;
    }

    std::string command = args[0];

    if (command == "help") {
        printHelp();
        return true;
    }

    if (command == "exit") {
        COMPLOG_INFO("[TESTER] Exiting...");
        if (d->client.isConnected()) {
            d->client.disconnect();
        }
        d->running = false;
        return false;
    }

    auto it = d->actions.find(command);
    if (it == d->actions.end()) {
        COMPLOG_ERROR("[TESTER] Unknown command:", command);
        COMPLOG_EMPTY("[TESTER] Type 'help' for list of available commands");
        return true;
    }

    TesterAction& action = *it->second;
    std::vector<std::string> actionArgs(args.begin() + 1, args.end());

    if (!action.validate(actionArgs)) {
        COMPLOG_ERROR("[TESTER] Invalid arguments for command:", command);
        COMPLOG_EMPTY("[TESTER] Usage:", action.getUsage());
        return true;
    }

    COMPLOG_EMPTY("--- Starting test:", command, "---");
    bool success = action.execute(d->client, actionArgs);

    if (success) {
        COMPLOG_EMPTY("=== Test PASSED:", command, "===");
    } else {
        COMPLOG_EMPTY("=== Test FAILED:", command, "===");
    }
    COMPLOG_EMPTY("");

    return true;
}

void AIBackendTester::run()
{
    printHelp();
    COMPLOG_EMPTY("");

    std::string line;
    while (d->running) {
        std::cout << "AIBackendTester> " << std::flush;
        if (!std::getline(std::cin, line)) {
            break;
        }

        if (line.empty()) {
            continue;
        }

        processLine(line);
    }
}