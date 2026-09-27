#pragma once

#include "actionbase.hpp"

#include <ProjectAgency/Exchange/WSEvent.h>

class SetConfigAction : public TesterAction
{
public:
    std::string getName() const override { return "set_config"; }
    std::string getDescription() const override { return "Set model configuration on AIBackend"; }
    std::string getUsage() const override { return "set_config [\"custom config text\"]"; }

    bool validate(const std::vector<std::string>& args) const override
    {
        return args.size() <= 1;
    }

    bool execute(WebSockets::Client& client, const std::vector<std::string>& args) override
    {
        COMPLOG_INFO("[TESTER] Starting AIAskSetConfig test...");

        if (!client.isConnected()) {
            COMPLOG_ERROR("[TESTER] Not connected to server");
            COMPLOG_EMPTY("[TESTER] Expected: connected state before set_config");
            return false;
        }

        std::string configText;
        if (!args.empty()) {
            configText = args[0];
            COMPLOG_INFO("[TESTER] Using custom config");
        } else {
            configText = "FROM qwen3.5\n"
                         "PARAMETER temperature 0.7\n"
                         "PARAMETER top_p 0.9\n"
                         "PARAMETER num_ctx 4096\n"
                         "SYSTEM \"\"\"You are a test assistant configured by AIBackendTester.\n"
                         "Answer every request concisely.\"\"\"";
            COMPLOG_INFO("[TESTER] Using default config");
        }

        COMPLOG_DEBUG("[TESTER] Config text:", configText);
        std::string payload = configText;

        auto state = createResponseState();
        setupResponseCallback(client, state, Exchange::Events::EventType::AIAskSetConfig);

        COMPLOG_INFO("[TESTER] Sending AIAskSetConfig event...");
        if (!sendWSEvent(client, Exchange::Events::EventType::AIAskSetConfig, payload)) {
            COMPLOG_ERROR("[TESTER] Failed to send AIAskSetConfig event");
            COMPLOG_EMPTY("[TESTER] Expected: AIAskSetConfig event sent successfully");
            return false;
        }
        COMPLOG_INFO("[TESTER] Waiting for config response...");

        if (!waitForResponse(state, 5000)) {
            COMPLOG_ERROR("[TESTER] AIAskSetConfig response timeout (5s)");
            COMPLOG_EMPTY("[TESTER] Expected: set_config response within 5 seconds");
            return false;
        }

        std::string result = state->payload;
        COMPLOG_EMPTY("[TESTER] AIAskSetConfig result:", result);

        if (result == "ok") {
            COMPLOG_OK("[TESTER] Model configuration updated successfully");
            return true;
        }

        COMPLOG_ERROR("[TESTER] Model configuration update failed, result:", result);
        COMPLOG_EMPTY("[TESTER] Expected: 'ok' for successful configuration update");
        return false;
    }
};