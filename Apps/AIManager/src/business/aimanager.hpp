#pragma once

#include <memory>
#include <string>
#include <vector>

#include "backendhandle/backendhandlerbuilder.hpp"
#include "backendhandle/aibackendhandler.hpp"

/**
 * @brief The AIManager class AIBackend handler class
 */
class AIManager
{
public:
    AIManager();
    ~AIManager();

    void setToken(const std::string& tokenString);
    void setPlanningModel(const std::string& modelName);

    bool init();
    void start();
    void stop();

    std::vector<std::shared_ptr<AIBackendHandler> > getBackends() const;

private:
    std::string m_token;
    std::string m_plannerModel;

    BackendHandlerBuilder m_backendBuilder;
    std::vector<std::shared_ptr<AIBackendHandler> > m_backends;
};
