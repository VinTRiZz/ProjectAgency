#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <sstream>

#include <Components/Network/ClientWS.h>

#include "actions/actionbase.hpp"

class AIBackendTester
{
public:
    AIBackendTester();
    ~AIBackendTester();

    void run();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void registerActions();
    void printHelp() const;
    bool processLine(const std::string& line);
    std::vector<std::string> parseArguments(const std::string& line) const;
};