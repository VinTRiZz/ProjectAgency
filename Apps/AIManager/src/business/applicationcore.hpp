#pragma once

#include <memory>
#include <string>

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
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void initDatabase();
};
