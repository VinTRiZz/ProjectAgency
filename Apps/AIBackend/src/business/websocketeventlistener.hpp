#pragma once

#include <ProjectAgency/WSEvent.h>
#include <functional>
#include <memory>

class WebsocketEventListener
{
public:
    WebsocketEventListener();
    ~WebsocketEventListener();

    /**
     * @brief setManagerToken   Set token of AIManager app to reject other connections
     * @param tokenString
     */
    void setManagerToken(const std::string& tokenString);

    /**
     * @brief listen    Start async listening for events
     * @param port      Listen port
     * @return          true if listen started
     */
    bool listen(uint16_t port);
    void stop();

    /**
     * @brief setEventCallback  Set callback for event type. Must not be called after @ref listen() method
     * @param evType            Type of events to process using callback
     * @param eventCallback     Callback to process events
     */
    void setEventCallback(
        DataObjects::Events::EventType evType,
        std::function<void(DataObjects::Events::WSEvent&&)>&& eventCallback);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void initClient();
    void initConnectionProcessing();
    void initMessageProcessing();
};
