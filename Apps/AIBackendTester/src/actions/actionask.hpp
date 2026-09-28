#pragma once

#include "actionbase.hpp"

class AskAction : public TesterAction
{
public:
    std::string getName() const override { return "ask"; }
    std::string getDescription() const override { return "Send AIAsk event with built-in or custom prompt"; }
    std::string getUsage() const override { return "ask [\"custom prompt text\"]"; }

    bool validate(const std::vector<std::string>& args) const override
    {
        return args.size() <= 1;
    }

    bool execute(WebSockets::Client& client, const std::vector<std::string>& args) override
    {
        COMPLOG_INFO("[TESTER] Starting AIAsk test...");

        if (!client.isConnected()) {
            COMPLOG_ERROR("[TESTER] Not connected to server");
            COMPLOG_EMPTY("[TESTER] Expected: connected state before ask");
            return false;
        }

        std::string promptText;
        if (!args.empty()) {
            promptText = args[0];
            COMPLOG_INFO("[TESTER] Using custom prompt:", promptText);
        } else {
            promptText = "Say 'Hello from AIBackendTester!' and nothing else. Do NOT add any extra text and do NOT think about answer.";
            COMPLOG_INFO("[TESTER] Using default prompt");
        }

        std::string payload = promptText;

        auto state = createResponseState();
        setupResponseCallback(client, state, Exchange::Events::EventType::AIAsk);

        COMPLOG_INFO("[TESTER] Sending AIAsk event...");
        if (!sendWSEvent(client, Exchange::Events::EventType::AIAsk, payload)) {
            COMPLOG_ERROR("[TESTER] Failed to send AIAsk event");
            COMPLOG_EMPTY("[TESTER] Expected: AIAsk event sent successfully");
            return false;
        }
        COMPLOG_INFO("[TESTER] AIAsk sent, waiting for response...");

        if (!waitForResponse(state, 60000)) {
            COMPLOG_ERROR("[TESTER] AIAsk response timeout (60s)");
            COMPLOG_EMPTY("[TESTER] Expected: AI response within 60 seconds");
            return false;
        }

        if (!state->success) {
            COMPLOG_ERROR("[TESTER] AIAsk failed:", state->errorText);
            COMPLOG_EMPTY("[TESTER] Expected: successful AI response");
            return false;
        }

        auto res = parseAskResponse(state->payload);
        displayAskResponse(res);

        if (res.pending) {
            COMPLOG_WARNING("[TESTER] AIAsk accepted but still pending. ID:", res.id);
        } else {
            COMPLOG_OK("[TESTER] AIAsk completed. ID:", res.id);
        }
        return true;
    }
};