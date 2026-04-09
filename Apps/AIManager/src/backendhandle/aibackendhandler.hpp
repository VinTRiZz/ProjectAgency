#pragma once

#include <memory>
#include <string>

#include <ProjectAgency/WSEvent.h>

class AIBackendHandler
{
public:
    explicit AIBackendHandler(const std::string& backendAddress);
    ~AIBackendHandler();

    void setToken(const std::string& token);

    void connect();
    void disconnect();
    bool isConnected() const;

    bool sendEvent(const DataObjects::Events::WSEvent& ev);

    void setDisplayName(const std::string& displayName);
    std::string_view getDisplayName() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
