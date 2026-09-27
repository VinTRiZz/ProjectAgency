#pragma once

#include "actionbase.hpp"

#include <ProjectAgency/Exchange/WSEvent.h>

class AskStatusAction : public TesterAction
{
public:
    std::string getName() const override { return "ask_status"; }
    std::string getDescription() const override { return "Check status of an AI ask request by ID"; }
    std::string getUsage() const override { return "ask_status <askId>"; }

    bool validate(const std::vector<std::string>& args) const override
    {
        return args.size() == 1 && !args[0].empty();
    }

    bool execute(WebSockets::Client& client, const std::vector<std::string>& args) override
    {
        COMPLOG_INFO("[TESTER] Starting AIAskStatus test...");

        if (!client.isConnected()) {
            COMPLOG_ERROR("[TESTER] Not connected to server");
            COMPLOG_EMPTY("[TESTER] Expected: connected state before ask_status");
            return false;
        }

        std::string payload = args[0];

        auto state = createResponseState();
        setupResponseCallback(client, state, Exchange::Events::EventType::AIAskStatus);

        COMPLOG_INFO("[TESTER] Sending AIAskStatus for ID:", args[0]);
        if (!sendWSEvent(client, Exchange::Events::EventType::AIAskStatus, payload)) {
            COMPLOG_ERROR("[TESTER] Failed to send AIAskStatus event");
            COMPLOG_EMPTY("[TESTER] Expected: AIAskStatus event sent successfully");
            return false;
        }
        COMPLOG_INFO("[TESTER] Waiting for status response...");

        if (!waitForResponse(state, 5000)) {
            COMPLOG_ERROR("[TESTER] AIAskStatus response timeout (5s)");
            COMPLOG_EMPTY("[TESTER] Expected: status response within 5 seconds");
            return false;
        }

        std::string status = state->payload;
        COMPLOG_EMPTY("[TESTER] AIAskStatus result:", status);

        if (status == "not found") {
            COMPLOG_WARNING("[TESTER] Ask ID not found on server");
            return true;
        }

        COMPLOG_OK("[TESTER] AIAskStatus test completed. ID:", args[0], "status:", status);
        return true;
    }
};