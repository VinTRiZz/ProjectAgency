#pragma once

#include <memory>
#include <string>

class AIManager
{
public:
    AIManager();
    ~AIManager();

    void initBackends();

    void setToken(const std::string& tokenString);

    void start(uint16_t apiPort);
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
