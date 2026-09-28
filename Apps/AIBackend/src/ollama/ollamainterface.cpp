#include "ollamainterface.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Network/ClientHTTP.h>
#include <Components/Ecosystem/Utility.h>

#include <nlohmann/json.hpp>
#include <boost/algorithm/string.hpp>


OllamaInterface::ResponsePtr OllamaInterface::Response::create(const std::shared_ptr<HTTP::Client>& pSession, AIObjects::AIRequest&& req)
{
    auto pRes = std::make_shared<OllamaInterface::Response>();
    pRes->m_pSession = pSession;
    pRes->m_id = Common::createRandomString(128);
    pRes->m_request = std::make_shared<AIObjects::AIRequest>(std::move(req));
    return pRes;
}

std::string OllamaInterface::Response::getId() const
{
    return m_id;
}

std::shared_ptr<AIObjects::AIRequest> OllamaInterface::Response::getRequest() const
{
    return m_request;
}

AIObjects::AIResponse OllamaInterface::Response::getResponse() const
{
    return m_responseFuture.get();
}

bool OllamaInterface::Response::isPending() const
{
    return isValid() && (
        std::future_status::ready !=
        m_responseFuture.wait_for(std::chrono::nanoseconds(0))
        );
}

bool OllamaInterface::Response::isValid() const
{
    return m_isValid;
}

void OllamaInterface::Response::setErrorText(const std::string &txt)
{
    m_errorText = txt;
}

std::string OllamaInterface::Response::getErrorText() const
{
    return m_errorText;
}

void OllamaInterface::Response::setPromise(const std::shared_ptr<std::promise<AIObjects::AIResponse> > &prom)
{
    m_responseProm = prom;
    m_responseFuture = m_responseProm->get_future();
    m_isValid = m_responseFuture.valid();
}

void OllamaInterface::Response::stop()
{
    if (!isPending()) {
        m_pSession->interruptRequestProcessing();
        m_responseProm->set_value({});
    }
}




struct OllamaInterface::Impl
{
    std::string m_host {"127.0.0.1"};
    uint16_t    m_port {11434};

    std::unordered_map<std::string, ResponsePtr> m_pendingRequests;
    std::function<void(const ResponsePtr&)> m_askEventCallback;
    mutable std::mutex m_requestMx;

    std::string generateRequestId() const {
        std::string res = Common::createRandomString(64);

        std::lock_guard<std::mutex> locker(m_requestMx);
        while (m_pendingRequests.count(res)) {
            res = Common::createRandomString(64);
        }
        return res;
    }
};

OllamaInterface::OllamaInterface() :
    d {new Impl}
{

}

OllamaInterface::~OllamaInterface()
{

}

void OllamaInterface::setAPIserver(const std::string &serverHost, uint16_t apiPort)
{
    d->m_host = serverHost;
    d->m_port = apiPort;
}

void OllamaInterface::setAskEventCallback(std::function<void (const ResponsePtr &)> &&cbk)
{
    d->m_askEventCallback = std::move(cbk);
}

OllamaInterface::ResponsePtr OllamaInterface::getRequest(const std::string_view &askId) const
{
    std::lock_guard<std::mutex> lock(d->m_requestMx);
    auto targetAsk = d->m_pendingRequests.find(askId.data());
    if (d->m_pendingRequests.end() == targetAsk) {
        return {};
    }
    return targetAsk->second;
}

bool OllamaInterface::interruptAsk(const std::string_view &askId)
{
    auto targetAsk = getRequest(askId);
    if (targetAsk) {
        targetAsk->stop();
        return true;
    }
    return false;
}

OllamaInterface::ResponsePtr OllamaInterface::ask(AIObjects::AIRequest&& req)
{
    auto pSession = std::make_shared<HTTP::Client>();
    auto pResp = Response::create(pSession, std::move(req));
    auto pPromise = std::make_shared<std::promise<AIObjects::AIResponse> >();
    pResp->setPromise(pPromise);

    {
        std::lock_guard<std::mutex> locker(d->m_requestMx);
        d->m_pendingRequests[pResp->getId()] = pResp;
    }

    std::thread requestTh([this, pSession, pResp, pPromise](){
        pSession->setClientName("AIBackend");
        pSession->setHost(d->m_host, d->m_port);

        // Prepare packet
        HTTP::Packet requestPacket;
        requestPacket.target = "/api/generate";
        requestPacket.bodyType = HTTP::Packet::BodyType::Json;
        requestPacket.body = pResp->getRequest()->toJson();

        // Request server for data
        auto responsePacket = pSession->request(HTTP::MethodType::Post, std::move(requestPacket));

        if (200 != responsePacket.statusCode) {
            COMPLOG_INFO("[OLLAMA] Request", pResp->getId(), "failed (code", responsePacket.statusCode, ")");
            pPromise->set_value({});
            pResp->setErrorText(responsePacket.body);
            d->m_askEventCallback(pResp);
            return;
        }

        AIObjects::AIResponse res;
        if (!res.readJson(responsePacket.body)) {
            COMPLOG_WARNING("[OLLAMA] Request", pResp->getId(), "failed to parse response json");
        }
        COMPLOG_INFO("[OLLAMA] Request", pResp->getId(), "complete");
        pPromise->set_value(std::move(res));
        d->m_askEventCallback(pResp);
    });
    requestTh.detach();

    return pResp;
}
