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

std::pair<unsigned, std::string> OllamaInterface::askSync(const std::string &modelName, std::string requestText)
{
    // Prepare request
    boost::algorithm::replace_all(requestText, "\"", "\\\"");
    boost::algorithm::replace_all(requestText, "\n", "\\n");

    // Prepare JSON
    nlohmann::json reqJson;
    reqJson["model"] = modelName;
    reqJson["prompt"] = requestText;
    reqJson["stream"] = false;

    // Prepare packet
    HTTP::Packet requestPacket;
    requestPacket.target = "/api/generate";
    requestPacket.bodyType = HTTP::Packet::BodyType::Json;
    requestPacket.body = reqJson.dump();

    // Request
    auto responsePacket = d->httpClient.request(HTTP::MethodType::Post, std::move(requestPacket));
    return std::make_pair(responsePacket.statusCode, responsePacket.body);
}

void OllamaInterface::setResponsePartCallback(const std::function<void (std::string &&, std::string &&, bool)> &&responsePartCallback)
{
    // TODO: Setup
}
