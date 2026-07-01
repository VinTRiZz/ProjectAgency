#pragma once

#include <ProjectAgency/Exchange/WSEvent.h>
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
     * @brief listen        Start async listening for events
     * @param port          Listen port
     * @param threadCount   Count of threads to listen
     * @return              true if listen started
     */
    bool listen(uint16_t port, uint8_t threadCount);
    void stop();

    /**
     * @brief setEventCallback  Set callback for event type. Must not be called after @ref listen() method
     * @param evType            Type of events to process using callback
     * @param eventCallback     Callback to process events
     */
    void setEventCallback(
        Exchange::Events::EventType evType,
        std::function<void(Exchange::Events::WSEvent&&)>&& eventCallback);

    void sendResponse(const std::string_view& respText);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void initClient();
    void initConnectionProcessing();
    void initMessageProcessing();
};
