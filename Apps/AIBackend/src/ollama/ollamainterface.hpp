#pragma once

#include <string>
#include <memory>
#include <future>

#include <ProjectAgency/AIObjects/AIResponse.h>
#include <ProjectAgency/AIObjects/AIRequest.h>

namespace HTTP {
class Client;
}

class OllamaInterface
{
public:
    OllamaInterface();
    ~OllamaInterface();

    class Response;
    using ResponsePtr = std::shared_ptr<Response>;

    /**
     * @brief The Response class Handler that used to wait for response
     */
    class Response
    {
    public:
        static ResponsePtr create(const std::shared_ptr<HTTP::Client>& pSession, AIObjects::AIRequest&& req);

        std::string getId() const;
        std::shared_ptr<AIObjects::AIRequest> getRequest() const;
        AIObjects::AIResponse getResponse() const;

        bool isPending() const;
        bool isValid() const;

        void setErrorText(const std::string& txt);
        std::string getErrorText() const;

        void setPromise(const std::shared_ptr<std::promise<AIObjects::AIResponse> >& prom);
        void stop();

    private:
        std::string m_id;
        bool        m_isValid {false};
        std::string m_errorText;
        std::shared_ptr<HTTP::Client> m_pSession;
        std::shared_ptr<AIObjects::AIRequest>                   m_request;
        std::shared_future<AIObjects::AIResponse>               m_responseFuture;
        std::shared_ptr<std::promise<AIObjects::AIResponse> >   m_responseProm;
    };

    void setAPIserver(const std::string& serverHost, uint16_t apiPort);
    void setAskEventCallback(std::function<void(const ResponsePtr&)>&& cbk);

    ResponsePtr ask(AIObjects::AIRequest&& req);
    ResponsePtr getRequest(const std::string_view& askId) const;
    bool interruptAsk(const std::string_view& askId);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
