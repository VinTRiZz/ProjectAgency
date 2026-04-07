#include "ollamainterface.hpp"

#include <Components/Network/ClientHTTP.h>

#include <nlohmann/json.hpp>
#include <boost/algorithm/string.hpp>

struct OllamaInterface::Impl
{
    HTTP::Client httpClient;
};

OllamaInterface::OllamaInterface() :
    d {new Impl}
{
    d->httpClient.setClientName("AIBackend");
}

OllamaInterface::~OllamaInterface()
{

}

void OllamaInterface::setAPIserver(const std::string &serverHost, uint16_t apiPort)
{
    d->httpClient.setHost(serverHost, apiPort);
}

DataObjects::AIResponse OllamaInterface::askSync(const DataObjects::AIRequest &req)
{
    // Prepare packet
    HTTP::Packet requestPacket;
    requestPacket.target = "/api/generate";
    requestPacket.bodyType = HTTP::Packet::BodyType::Json;
    requestPacket.body = req.toJson();

    // Request
    auto responsePacket = d->httpClient.request(HTTP::MethodType::Post, std::move(requestPacket));

    // Parse answer
    DataObjects::AIResponse res;
    res.readJson(responsePacket.body);
    return res;
}

void OllamaInterface::setResponsePartCallback(const std::function<void (std::string &&, std::string &&, bool)> &&responsePartCallback)
{
    // TODO: Setup
}
