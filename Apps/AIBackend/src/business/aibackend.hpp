#pragma once

#include <string>
#include <memory>

/**
 * @brief The AIBackend class Main instance of application
 */
class AIBackend
{
public:
    AIBackend();
    ~AIBackend();

    void start(const std::string& managerToken,
               uint16_t eventListenPort,
               const std::string& ollamaServerAddress, uint16_t ollamaAPIPort);
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    // Event processing
    void initEventProcessing();
    void initEventProcessingAIAsk();

    void initOllamaInterface();
};
