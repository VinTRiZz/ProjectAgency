#pragma once

#include "actionbase.hpp"

class DisconnectAction : public TesterAction
{
public:
    std::string getName() const override { return "disconnect"; }
    std::string getDescription() const override { return "Disconnect from AIBackend WebSocket server"; }
    std::string getUsage() const override { return "disconnect"; }

    bool validate(const std::vector<std::string>& args) const override
    {
        return args.empty();
    }

    bool execute(WebSockets::Client& client, const std::vector<std::string>&) override
    {
        COMPLOG_INFO("[TESTER] Starting disconnect test...");

        if (!client.isConnected()) {
            COMPLOG_WARNING("[TESTER] Not connected, nothing to disconnect");
            return true;
        }

        COMPLOG_INFO("[TESTER] Disconnecting from server...");
        client.disconnect();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        if (client.isConnected()) {
            COMPLOG_ERROR("[TESTER] Disconnect failed - still connected");
            COMPLOG_EMPTY("[TESTER] Expected: disconnected from server");
            return false;
        }

        COMPLOG_OK("[TESTER] Successfully disconnected");
        return true;
    }
};