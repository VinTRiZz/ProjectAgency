#pragma once

#include <string>
#include <memory>
#include <functional>

#include <ProjectAgency/AIRequest.h>
#include <ProjectAgency/AIResponse.h>

class OllamaInterface
{
public:
    OllamaInterface();
    ~OllamaInterface();

    void setAPIserver(const std::string& serverHost, uint16_t apiPort);

    DataObjects::AIResponse askSync(const DataObjects::AIRequest& req);
    void ask(const DataObjects::AIRequest& req);

    void setResponseCallback(const std::function<void(std::optional<DataObjects::AIResponse>&&)>&& responseCallback);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
