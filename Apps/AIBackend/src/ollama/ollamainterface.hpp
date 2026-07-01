#pragma once

#include <string>
#include <memory>
#include <functional>

#include <ProjectAgency/AIObjects/AIResponse.h>
#include <ProjectAgency/AIObjects/AIRequest.h>

class OllamaInterface
{
public:
    OllamaInterface();
    ~OllamaInterface();

    void setAPIserver(const std::string& serverHost, uint16_t apiPort);

    AIObjects::AIResponse askSync(const AIObjects::AIRequest& req);
    void ask(const AIObjects::AIRequest& req);

    void askInterrupt();

    void setResponseCallback(const std::function<void(std::optional<AIObjects::AIResponse>&&)>&& responseCallback);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
