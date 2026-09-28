#pragma once

#include "actionbase.hpp"

class AskStatusAction : public TesterAction
{
public:
    std::string getName() const override { return "ask_status"; }
    std::string getDescription() const override
    {
        return "Comprehensive status test: send ask, poll status, wait, poll again, check completion";
    }
    std::string getUsage() const override { return "ask_status [\"custom prompt text\"]"; }

    bool validate(const std::vector<std::string>& args) const override
    {
        return args.size() <= 1;
    }

    bool execute(WebSockets::Client& client, const std::vector<std::string>& args) override
    {
        COMPLOG_INFO("[TESTER] Starting comprehensive AIAskStatus test...");

        if (!client.isConnected()) {
            COMPLOG_ERROR("[TESTER] Not connected to server");
            COMPLOG_EMPTY("[TESTER] Expected: connected state before ask_status");
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

        // Phase 1: Send AIAsk, wait for initial response to get the ask ID
        COMPLOG_INFO("[TESTER] Phase 1: Sending AIAsk...");
        auto askState = createResponseState();
        setupResponseCallback(client, askState, Exchange::Events::EventType::AIAsk);

        if (!sendWSEvent(client, Exchange::Events::EventType::AIAsk, promptText)) {
            COMPLOG_ERROR("[TESTER] Failed to send AIAsk event");
            COMPLOG_EMPTY("[TESTER] Expected: AIAsk event sent successfully");
            return false;
        }

        if (!waitForResponse(askState, 10000)) {
            COMPLOG_ERROR("[TESTER] AIAsk initial response timeout (10s)");
            COMPLOG_EMPTY("[TESTER] Expected: initial AIAsk response within 10 seconds");
            return false;
        }

        auto res = parseAskResponse(askState->payload);
        displayAskResponse(res);

        if (res.id == "unknown") {
            COMPLOG_ERROR("[TESTER] Could not extract ask ID from response");
            return false;
        }

        // If already completed, no need for comprehensive polling
        if (!res.pending) {
            COMPLOG_OK("[TESTER] Ask completed immediately, skipping polling");
            return true;
        }

        // Phase 2: Poll status immediately
        COMPLOG_EMPTY("");
        COMPLOG_INFO("[TESTER] Phase 2: Polling status immediately...");
        if (!pollStatusOnce(client, res.id, 5000)) {
            return false;
        }

        // Phase 3: Wait 1s, poll again
        COMPLOG_EMPTY("");
        COMPLOG_INFO("[TESTER] Phase 3: Waiting 1 second...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        COMPLOG_INFO("[TESTER] Phase 3: Polling status again...");
        if (!pollStatusOnce(client, res.id, 5000)) {
            return false;
        }

        // Phase 4: Wait for the completed AIAsk response
        COMPLOG_EMPTY("");
        COMPLOG_INFO("[TESTER] Phase 4: Waiting for AIAsk completion response...");
        auto completeState = createResponseState();
        setupResponseCallback(client, completeState, Exchange::Events::EventType::AIAsk);

        if (!waitForResponse(completeState, 60000)) {
            COMPLOG_ERROR("[TESTER] AIAsk completion response timeout (60s)");
            COMPLOG_EMPTY("[TESTER] Expected: AI response to complete within 60 seconds");
            return false;
        }

        auto completeRes = parseAskResponse(completeState->payload);
        displayAskResponse(completeRes);

        // Phase 5: Final status poll
        COMPLOG_EMPTY("");
        COMPLOG_INFO("[TESTER] Phase 5: Final status poll after completion...");
        if (!pollStatusOnce(client, res.id, 5000)) {
            return false;
        }

        COMPLOG_OK("[TESTER] Comprehensive status test completed successfully");
        return true;
    }

private:
    bool pollStatusOnce(WebSockets::Client& client, const std::string& askId, int timeoutMs)
    {
        auto state = createResponseState();
        setupResponseCallback(client, state, Exchange::Events::EventType::AIAskStatus);

        if (!sendWSEvent(client, Exchange::Events::EventType::AIAskStatus, askId)) {
            COMPLOG_ERROR("[TESTER] Failed to send AIAskStatus event");
            return false;
        }

        if (!waitForResponse(state, timeoutMs)) {
            COMPLOG_ERROR("[TESTER] AIAskStatus response timeout (", timeoutMs, "ms)");
            COMPLOG_EMPTY("[TESTER] Expected: status response for ID:", askId);
            return false;
        }

        displayStatusResponse(state->payload);
        return true;
    }
};