#pragma once

#include <memory>
#include <string>
#include <vector>

#include "backendhandle/aibackendhandler.hpp"

#include "database/recordmanager.hpp"

/**
 * @brief The AIManager class AIBackend handler class
 */
class AIManager
{
public:
    AIManager();
    ~AIManager();

    void setRecordManager(const Database::RecordManagerPtr& pManager);

    void setToken(const std::string& tokenString);
    void setInputModel(const std::string& modelName);

    void init();
    void start();
    void stop();

    std::vector<std::shared_ptr<AIBackendHandler> > getBackends() const;

private:
    std::string m_token;
    std::string m_plannerModel;

    Database::RecordManagerPtr m_pRecordManager;

    std::vector<std::shared_ptr<AIBackendHandler> > m_backends;
};
