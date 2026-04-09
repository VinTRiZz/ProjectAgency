#pragma once

#include <memory>
#include <string>
#include <stdint.h>

#include <nlohmann/json.hpp>

class AIBackendHandler;

class BackendHandlerBuilder
{
public:
    BackendHandlerBuilder();
    ~BackendHandlerBuilder();

    std::shared_ptr<AIBackendHandler> create(const std::string& hostname, uint16_t port);
    std::shared_ptr<AIBackendHandler> fromConfig(const nlohmann::json& iJson);
};
