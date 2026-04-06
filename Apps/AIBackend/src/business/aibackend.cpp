#include "aibackend.hpp"

#include "websocketeventlistener.hpp"
#include "ollama/ollamainterface.hpp"

struct AIBackend::Impl
{
    WebsocketEventListener  eventListener;
    OllamaInterface         ollamaInterface;
};

AIBackend::AIBackend() :
    d {new Impl}
{
    initEventProcessing();
}

AIBackend::~AIBackend()
{

}

void AIBackend::setup(const std::string &managerToken, uint16_t eventPort, uint16_t ollamaPort)
{

}

void AIBackend::stop()
{

}

void AIBackend::initEventProcessing()
{

}
