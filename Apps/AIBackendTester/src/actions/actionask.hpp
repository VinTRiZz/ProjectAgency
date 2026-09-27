#pragma once

#include "actionbase.hpp"

#include <nlohmann/json.hpp>

#include <ProjectAgency/Exchange/WSEvent.h>

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
            promptText = "Say 'Hello from AIBackendTester!' and nothing else. Do not add any extra text.";
            COMPLOG_INFO("[TESTER] Using default prompt");
        }

        nlohmann::json reqJson;
        reqJson["model"] = "qwen3.5";
        reqJson["prompt"] = promptText;
        reqJson["stream"] = false;
        std::string payload = reqJson.dump();

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

        std::string decodedPayload = state->payload;
        COMPLOG_INFO("[TESTER] Raw response payload:", decodedPayload);

        try {
            auto respJson = nlohmann::json::parse(decodedPayload);
            bool pending = respJson.value("pending", true);
            std::string respId = respJson.value("id", "unknown");

            if (pending) {
                COMPLOG_WARNING("[TESTER] AIAsk response indicates pending. ID:", respId);
                COMPLOG_INFO("[TESTER] Use 'ask_status", respId, "' to check status");
                COMPLOG_EMPTY("[TESTER] Test passed: AIAsk accepted (pending), ID:", respId);
            } else {
                if (respJson.contains("data") && respJson["data"].is_string()) {
                    std::string dataStr = respJson["data"].get<std::string>();
                    COMPLOG_OK("[TESTER] AIAsk completed. Response ID:", respId);
                    COMPLOG_EMPTY("[TESTER] Test passed: AIAsk completed successfully. Data:", dataStr);
                } else {
                    COMPLOG_OK("[TESTER] AIAsk completed. Response ID:", respId);
                    COMPLOG_EMPTY("[TESTER] Test passed: AIAsk completed successfully");
                }
            }
        } catch (const nlohmann::json::exception& e) {
            COMPLOG_ERROR("[TESTER] Failed to parse AIAsk response JSON:", e.what());
            COMPLOG_EMPTY("[TESTER] Expected: valid JSON response with 'id' and 'pending' fields");
            return false;
        }

        return true;
    }
};