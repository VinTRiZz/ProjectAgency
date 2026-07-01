#include "ollamainterface.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Network/ClientHTTP.h>

#include <nlohmann/json.hpp>
#include <boost/algorithm/string.hpp>

struct OllamaInterface::Impl
{
    HTTP::Client httpClient;
    std::function<void(std::optional<AIObjects::AIResponse>&&)> responseCallback;
};

OllamaInterface::OllamaInterface() :
    d {new Impl}
{
    d->httpClient.setClientName("AIBackend");

    // TODO: Remove? (debug needs)
    // d->httpClient.setLoggingEnabled(true);
}

OllamaInterface::~OllamaInterface()
{

}

void OllamaInterface::setAPIserver(const std::string &serverHost, uint16_t apiPort)
{
    d->httpClient.setHost(serverHost, apiPort);
}

AIObjects::AIResponse OllamaInterface::askSync(const AIObjects::AIRequest &req)
{
    // Prepare packet
    HTTP::Packet requestPacket;
    requestPacket.target = "/api/generate";
    requestPacket.bodyType = HTTP::Packet::BodyType::Json;
    requestPacket.body = req.toJson();

    // Request
    COMPLOG_INFO("[OLLAMA] Model answer generation started...");
    auto responsePacket = d->httpClient.request(HTTP::MethodType::Post, std::move(requestPacket));
    COMPLOG_INFO("[OLLAMA] Model answer generation complete");

    // Parse answer
    AIObjects::AIResponse res;
    res.readJson(responsePacket.body);
    return res;
}

void OllamaInterface::ask(const AIObjects::AIRequest &req)
{
    // Prepare packet
    HTTP::Packet requestPacket;
    requestPacket.target = "/api/generate";
    requestPacket.bodyType = HTTP::Packet::BodyType::Json;
    requestPacket.body = req.toJson();

    // Request
    COMPLOG_INFO("[OLLAMA] Model async answer generation started...");
    d->httpClient.requestAsync(
        HTTP::MethodType::Post,
        std::move(requestPacket),
        [this](auto&& responseOpt){
            if (!d->responseCallback) {
                COMPLOG_WARNING("[OLLAMA] Model async answer ignored (no processor set)");
                return;
            }

            if (!responseOpt.has_value()) {
                d->responseCallback(std::nullopt);
                COMPLOG_WARNING("[OLLAMA] Model async answer receive failed");
                return;
            }

            AIObjects::AIResponse res;
            res.readJson(responseOpt->body);
            COMPLOG_INFO("[OLLAMA] Model async answer generation complete");
            d->responseCallback(res);
    });
}

void OllamaInterface::askInterrupt()
{
    d->httpClient.interruptRequestProcessing();
}

void OllamaInterface::setResponseCallback(const std::function<void (std::optional<AIObjects::AIResponse> &&)> &&responseCallback)
{
    d->responseCallback = std::move(responseCallback);
}
