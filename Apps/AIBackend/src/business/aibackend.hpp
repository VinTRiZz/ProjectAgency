#pragma once

#include <string>
#include <memory>

class AIBackend
{
public:
    AIBackend();
    ~AIBackend();

    void setup(const std::string& managerToken, uint16_t eventPort, uint16_t ollamaPort);
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void initEventProcessing();
};
