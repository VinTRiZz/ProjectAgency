#pragma once

#include <string>
#include <memory>
#include <functional>

class OllamaInterface
{
public:
    OllamaInterface();
    ~OllamaInterface();

    void setAPIserver(const std::string& serverHost, uint16_t apiPort);

    std::pair<unsigned, std::string> askSync(const std::string& modelName, std::string requestText);

    void setResponsePartCallback(const std::function<void(std::string&&, std::string&&, bool)>&& responsePartCallback);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
