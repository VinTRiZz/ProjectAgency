#include "backendhandlerbuilder.hpp"

#include <Components/Logger/Logger.h>

#include "aibackendhandler.hpp"

BackendHandlerBuilder::BackendHandlerBuilder()
{

}

BackendHandlerBuilder::~BackendHandlerBuilder()
{

}

std::shared_ptr<AIBackendHandler> BackendHandlerBuilder::create(const std::string &hostname, uint16_t port)
{
    auto pHandler = std::make_shared<AIBackendHandler>(hostname + ":" + std::to_string(port));

    // TODO: Configure?

    return pHandler;
}

std::shared_ptr<AIBackendHandler> BackendHandlerBuilder::fromConfig(const nlohmann::json &iJson)
{
    try {
        std::string hostname    = iJson["host"];
        uint16_t    port        = iJson["port"];
        std::string name        = iJson["display_name"];

        auto pHandler = std::make_shared<AIBackendHandler>(hostname + ":" + std::to_string(port));

        pHandler->setDisplayName(name);

        // TODO: Configure?

        return pHandler;

    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_WARNING("AIBackend config parse exception:", ex.what());
    }
    return {};
}
