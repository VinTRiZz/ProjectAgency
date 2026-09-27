#pragma once

#include "actionbase.hpp"

#include <ProjectAgency/Exchange/WSEvent.h>

class AskInterruptAction : public TesterAction
{
public:
    std::string getName() const override { return "ask_interrupt"; }
    std::string getDescription() const override { return "Interrupt an ongoing AI ask by ID"; }
    std::string getUsage() const override { return "ask_interrupt <askId>"; }

    bool validate(const std::vector<std::string>& args) const override
    {
        return args.size() == 1 && !args[0].empty();
    }

    bool execute(WebSockets::Client& client, const std::vector<std::string>& args) override
    {
        COMPLOG_INFO("[TESTER] Starting AIAskInterrupt test...");

        if (!client.isConnected()) {
            COMPLOG_ERROR("[TESTER] Not connected to server");
            COMPLOG_EMPTY("[TESTER] Expected: connected state before ask_interrupt");
            return false;
        }

        std::string payload = args[0];

        auto state = createResponseState();
        setupResponseCallback(client, state, Exchange::Events::EventType::AIAskInterrupt);

        COMPLOG_INFO("[TESTER] Sending AIAskInterrupt for ID:", args[0]);
        if (!sendWSEvent(client, Exchange::Events::EventType::AIAskInterrupt, payload)) {
            COMPLOG_ERROR("[TESTER] Failed to send AIAskInterrupt event");
            COMPLOG_EMPTY("[TESTER] Expected: AIAskInterrupt event sent successfully");
            return false;
        }
        COMPLOG_INFO("[TESTER] Waiting for interrupt response...");

        if (!waitForResponse(state, 5000)) {
            COMPLOG_ERROR("[TESTER] AIAskInterrupt response timeout (5s)");
            COMPLOG_EMPTY("[TESTER] Expected: interrupt response within 5 seconds");
            return false;
        }

        std::string result = state->payload;
        COMPLOG_EMPTY("[TESTER] AIAskInterrupt result:", result);

        if (result == "ok") {
            COMPLOG_OK("[TESTER] Successfully interrupted ask ID:", args[0]);
            return true;
        }

        COMPLOG_WARNING("[TESTER] Ask interrupt returned:", result, "for ID:", args[0]);
        COMPLOG_EMPTY("[TESTER] Expected: 'ok' for successful interrupt");
        return false;
    }
};