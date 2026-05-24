#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ProjectAgency/BackendDisplayInfo.h>

/**
 * @brief The AIManager class Main instance of application
 */
class AIManager
{
public:
    AIManager();
    ~AIManager();

    void initBackends();

    void setToken(const std::string& tokenString);
    void setPlanningModel(const std::string& modelName);

    void start(uint16_t apiPort);
    void stop();

    std::vector<DataObjects::BackendDisplayInfo> getBackends() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
