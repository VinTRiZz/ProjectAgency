#pragma once

#include <memory>
#include <string>

#include <ProjectAgency/Exchange/DatabaseConfiguration.h>

/**
 * @brief The ApplicationCore class Core, started from main function
 */
class ApplicationCore
{
public:
    ApplicationCore();
    ~ApplicationCore();

    void setToken(const std::string& tokenString);

    bool init();

    void start(uint16_t apiPort);
    bool isRunning() const;
    uint16_t getPort() const;
    void stop();

    void setDatabaseConfiguration(const Exchange::DatabaseConfiguration& dbConfig);
    Exchange::DatabaseConfiguration getDatabaseConfiguration() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void initDatabase();
};
