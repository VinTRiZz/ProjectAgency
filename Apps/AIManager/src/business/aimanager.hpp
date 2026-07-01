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
    Database::RecordManagerPtr getRecordManager() const;

    void setToken(const std::string& tokenString);
    void setInputModel(const std::string& modelName);

    void init();
    void start();
    void stop();

    void setCurrentTask(const std::string& taskText);
    std::string getCurrentTask() const;
    bool isSolvingTask() const;
    void stopCurrentTask();

    bool addBackend(const DBRecords::BackendInfoPtr& backendInfo);
    bool updateBackend(const DBRecords::BackendInfoPtr& backendInfo);
    std::vector<std::shared_ptr<AIBackendHandler> > getBackends() const;
    void removeBackend(const DBRecords::BackendInfo::id_t& backendId);

private:
    std::string m_token;
    std::string m_inputModelName;
    std::string m_currentTask;

    Database::RecordManagerPtr m_pRecordManager;
    std::vector<std::shared_ptr<AIBackendHandler> > m_backends;
};
