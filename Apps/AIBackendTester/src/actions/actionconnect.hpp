#pragma once

#include "actionbase.hpp"

class ConnectAction : public TesterAction
{
public:
    static constexpr auto DEFAULT_HOST     = "127.0.0.1";
    static constexpr uint16_t DEFAULT_PORT = 9010;
    static constexpr auto DEFAULT_TOKEN    = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    std::string getName() const override { return "connect"; }
    std::string getDescription() const override
    {
        return "Connect to AIBackend WebSocket server. Default: "
               + std::string(DEFAULT_HOST) + ":" + std::to_string(DEFAULT_PORT);
    }
    std::string getUsage() const override
    {
        return "connect [host=" + std::string(DEFAULT_HOST)
               + "] [port=" + std::to_string(DEFAULT_PORT)
               + "] [token=<64hex>]";
    }

    bool validate(const std::vector<std::string>& args) const override
    {
        if (args.size() > 3) {
            return false;
        }
        return true;
    }

    bool execute(WebSockets::Client& client, const std::vector<std::string>& args) override
    {
        COMPLOG_INFO("[TESTER] Starting connect test...");

        std::string host = DEFAULT_HOST;
        uint16_t port = DEFAULT_PORT;
        std::string token = DEFAULT_TOKEN;

        if (args.size() >= 1) host = args[0];
        if (args.size() >= 2) port = static_cast<uint16_t>(std::stoul(args[1]));
        if (args.size() >= 3) token = args[2];

        COMPLOG_INFO("[TESTER] Target server:", host, ":", port);
        std::string resource = "/?manager=" + token;
        COMPLOG_INFO("[TESTER] WS resource:", resource);

        COMPLOG_INFO("[TESTER] Connecting to server...");
        try {
            client.connect(host, port, resource);
        } catch (const std::exception& e) {
            COMPLOG_ERROR("[TESTER] Connection failed with exception:", e.what());
            COMPLOG_EMPTY("[TESTER] Expected: successful connection to", host, ":", port);
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (!client.isConnected()) {
            COMPLOG_ERROR("[TESTER] Connection failed - not connected after connect call");
            COMPLOG_EMPTY("[TESTER] Expected: successful connection to", host, ":", port);
            return false;
        }

        COMPLOG_OK("[TESTER] Successfully connected to", host, ":", port);
        return true;
    }
};