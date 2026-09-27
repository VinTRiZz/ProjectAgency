#pragma once

#include "actionbase.hpp"

class PingAction : public TesterAction
{
public:
    std::string getName() const override { return "ping"; }
    std::string getDescription() const override { return "Test WebSocket ping/pong with server"; }
    std::string getUsage() const override { return "ping [bytes=8] [timeoutMs=1000]"; }

    bool validate(const std::vector<std::string>& args) const override
    {
        if (args.size() > 2) {
            return false;
        }
        if (args.size() >= 1) {
            try {
                auto val = std::stoul(args[0]);
                if (val < 8) return false;
            } catch (...) { return false; }
        }
        if (args.size() >= 2) {
            try {
                std::stoul(args[1]);
            } catch (...) { return false; }
        }
        return true;
    }

    bool execute(WebSockets::Client& client, const std::vector<std::string>& args) override
    {
        COMPLOG_INFO("[TESTER] Starting ping test...");

        if (!client.isConnected()) {
            COMPLOG_ERROR("[TESTER] Not connected to server");
            COMPLOG_EMPTY("[TESTER] Expected: connected state before ping");
            return false;
        }

        size_t bytes = 8;
        int timeoutMs = 1000;
        if (args.size() >= 1) bytes = std::stoul(args[0]);
        if (args.size() >= 2) timeoutMs = std::stoi(args[1]);

        COMPLOG_INFO("[TESTER] Sending ping (bytes:", bytes, ", timeout:", timeoutMs, "ms)...");

        try {
            auto pingFuture = client.ping(bytes, timeoutMs);
            auto status = pingFuture.wait_for(std::chrono::milliseconds(timeoutMs + 1000));
            if (status != std::future_status::ready) {
                COMPLOG_ERROR("[TESTER] Ping timeout");
                COMPLOG_EMPTY("[TESTER] Expected: ping response within", timeoutMs, "ms");
                return false;
            }
            int result = pingFuture.get();
            if (result < 0) {
                COMPLOG_ERROR("[TESTER] Ping failed (no pong received)");
                COMPLOG_EMPTY("[TESTER] Expected: successful ping/pong with response time > 0");
                return false;
            }
            COMPLOG_OK("[TESTER] Ping success, response time:", result, "ms");
        } catch (const std::exception& e) {
            COMPLOG_ERROR("[TESTER] Ping exception:", e.what());
            COMPLOG_EMPTY("[TESTER] Expected: successful ping without exceptions");
            return false;
        }

        return true;
    }
};