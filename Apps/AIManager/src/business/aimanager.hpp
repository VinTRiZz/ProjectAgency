#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ProjectAgency/DB/RecordManager.h>

#include "backendhandle/aibackendhandler.hpp"

/**
 * @brief The AIManager class AIBackend handler class
 */
class AIManager
{
public:
    void setRecordManager(const Database::RecordManagerPtr& pManager);

    void setToken(const std::string& tokenString);
    void setInputModel(const std::string& modelName);

    void init();
    void start();
    void stop();

    void setCurrentTask(const std::string& taskText);

    std::vector<std::shared_ptr<AIBackendHandler> > getBackends() const;

private:
    std::string m_token;
    std::string m_inputModelName;

    Database::RecordManagerPtr m_pRecordManager;
    std::vector<std::shared_ptr<AIBackendHandler> > m_backends;
};
