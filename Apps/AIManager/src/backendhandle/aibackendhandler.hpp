#pragma once

#include <memory>
#include <string>
#include <functional>

#include <ProjectAgency/WSEvent.h>

class AIBackendHandler
{
public:
    explicit AIBackendHandler(const std::string& backendAddress);
    ~AIBackendHandler();

    // Common connection things
    void setToken(const std::string& token);
    void connect();
    void disconnect();
    bool isConnected() const;

    // Event processing (mostly used by internal)
    using EventCallback_t = std::function<void(DataObjects::Events::WSEvent&&)>;
    bool sendEvent(const DataObjects::Events::WSEvent& ev);
    void setEventCallback(DataObjects::Events::EventType etype, EventCallback_t&& cbk);

    // Display name for GUI or other
    void setDisplayName(const std::string& displayName);
    std::string_view getDisplayName() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
