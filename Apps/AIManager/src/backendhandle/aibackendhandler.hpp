#pragma once

#include <memory>
#include <functional>

#include <ProjectAgency/Exchange/Types.h>
#include <ProjectAgency/Exchange/WSEvent.h>

#include <ProjectAgency/DB/AIBackendInfo.h>
#include <ProjectAgency/DB/AIRole.h>

/**
 * @brief The AIBackendHandler class Interface to control remote AIBackend instance
 */
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
    using EventCallback_t = std::function<void(Exchange::Events::WSEvent&&)>;
    bool sendEvent(const Exchange::Events::WSEvent& ev);
    void setEventCallback(Exchange::Events::EventType etype, EventCallback_t&& cbk);

    // Backend's AI handling
    void setInfo(const DBRecords::AIBackendInfoPtr& info);
    DBRecords::AIBackendInfoPtr getInfo() const;
    void setModelRole(const DBRecords::AIRolePtr& aiRole);
    DBRecords::AIRolePtr getModelRole() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
