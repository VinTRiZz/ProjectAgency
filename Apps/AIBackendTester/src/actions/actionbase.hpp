#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <future>
#include <chrono>
#include <atomic>
#include <thread>

#include <Components/Network/ClientWS.h>
#include <ProjectAgency/Exchange/WSEvent.h>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

#include <nlohmann/json.hpp>

struct AskResponseData {
    std::string id;
    bool pending {true};
    std::string errorText;
    std::string data;
};

inline AskResponseData parseAskResponse(const std::string& payload)
{
    AskResponseData res;
    try {
        auto respJson = nlohmann::json::parse(payload);
        res.id = respJson.value("id", "unknown");
        res.pending = respJson.value("pending", true);
        res.errorText = respJson.value("error_text", "");
        if (respJson.contains("data")) {
            if (respJson["data"].is_string()) {
                res.data = respJson["data"].get<std::string>();
            } else if (!respJson["data"].is_null()) {
                res.data = respJson["data"].dump();
            }
        }
    } catch (const nlohmann::json::exception& e) {
        COMPLOG_ERROR("[TESTER] Failed to parse ask response JSON:", e.what());
    }
    return res;
}

inline void displayAskResponse(const AskResponseData& res)
{
    COMPLOG_EMPTY("--- AIAsk Response ---");
    COMPLOG_EMPTY("  ID:", res.id);
    COMPLOG_EMPTY("  Pending:", res.pending ? "yes" : "no");
    if (!res.errorText.empty()) {
        COMPLOG_EMPTY("  Error:", res.errorText);
    }
    if (!res.pending && !res.data.empty()) {
        COMPLOG_EMPTY("  Data:", res.data);
    }
    COMPLOG_EMPTY("-----------------------");
}

inline void displayStatusResponse(const std::string& payload)
{
    COMPLOG_EMPTY("--- AIAskStatus Response ---");
    if (payload == "not found") {
        COMPLOG_EMPTY("  Status: not found");
    } else {
        try {
            auto j = nlohmann::json::parse(payload);
            COMPLOG_EMPTY("  Pending:", j.value("pending", false) ? "yes" : "no");
            std::string err = j.value("error_text", "");
            if (!err.empty()) {
                COMPLOG_EMPTY("  Error:", err);
            }
        } catch (...) {
            COMPLOG_EMPTY("  Raw:", payload);
        }
    }
    COMPLOG_EMPTY("----------------------------");
}

class TesterAction
{
public:
    virtual ~TesterAction() = default;

    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool validate(const std::vector<std::string>& args) const = 0;
    virtual bool execute(WebSockets::Client& client, const std::vector<std::string>& args) = 0;

protected:
    struct WSResponseShared {
        std::promise<void> readyPromise;
        std::atomic<bool> readyFlag {false};
        bool success {false};
        Exchange::Events::EventType type {Exchange::Events::EventType::EtcUnknown};
        std::string payload;
        std::string errorText;
    };

    bool sendWSEvent(WebSockets::Client& client, Exchange::Events::EventType evType,
                     const std::string& payload)
    {
        Exchange::Events::WSEvent ev(evType);
        ev.setPayload(payload);
        auto jsonStr = ev.toJson();
        COMPLOG_DEBUG("[TESTER] Sending WS event type:", static_cast<int>(evType),
                      "payload size:", payload.size());
        return client.sendJson(std::move(jsonStr));
    }

    std::shared_ptr<WSResponseShared> createResponseState()
    {
        return std::make_shared<WSResponseShared>();
    }

    void setupResponseCallback(WebSockets::Client& client,
                               std::shared_ptr<WSResponseShared> state,
                               Exchange::Events::EventType expectedType)
    {
        client.setReceiveCallback([state, expectedType](std::string&& data) {
            if (state->readyFlag.load()) {
                return;
            }
            Exchange::Events::WSEvent ev;
            if (!ev.readJson(data)) {
                COMPLOG_DEBUG("[TESTER] Ignoring non-WSEvent message");
                return;
            }
            if (ev.getType() != expectedType) {
                COMPLOG_DEBUG("[TESTER] Ignoring WSEvent of unexpected type:",
                              static_cast<int>(ev.getType()),
                              "expected:", static_cast<int>(expectedType));
                return;
            }
            state->success = true;
            state->type = ev.getType();
            state->payload = std::string(ev.getPayload());
            state->readyFlag.store(true);
            state->readyPromise.set_value();
        });
    }

    bool waitForResponse(std::shared_ptr<WSResponseShared> state, int timeoutMs = 5000)
    {
        if (state->readyFlag.load()) {
            return true;
        }
        auto status = state->readyPromise.get_future().wait_for(
            std::chrono::milliseconds(timeoutMs));
        if (status != std::future_status::ready) {
            state->success = false;
            state->errorText = "Response timeout (" + std::to_string(timeoutMs) + "ms)";
            return false;
        }
        return true;
    }
};