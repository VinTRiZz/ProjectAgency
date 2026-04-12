#pragma once

#include "backendhandle/aibackendhandler.hpp"

#include <vector>

namespace Business
{

class LogicalCore
{
public:
    LogicalCore(std::vector<std::shared_ptr<AIBackendHandler> >& backends);

    void processPrompt(const std::string& userPrompt);
    void interrupt();

private:
    std::vector<std::shared_ptr<AIBackendHandler> >& m_backendVector;
};

}