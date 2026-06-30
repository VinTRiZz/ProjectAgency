#pragma once

#include <memory>
#include <functional>

#include <ProjectAgency/WSEvent.h>
#include <ProjectAgency/DB/BackendInfo.h>

class AIBackendHandler
{
public:
    AIBackendHandler();
    ~AIBackendHandler();

    // Common connection things
    void connect();
    void disconnect();
    bool isConnected() const;

    // Event processing (mostly used by internal)
    using EventCallback_t = std::function<void(DataObjects::Events::WSEvent&&)>;
    bool sendEvent(const DataObjects::Events::WSEvent& ev);
    void setEventCallback(DataObjects::Events::EventType etype, EventCallback_t&& cbk);

    DBRecords::BackendInfo& getInfo();
    const DBRecords::BackendInfo& getInfo() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
