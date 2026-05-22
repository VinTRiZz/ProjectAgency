#include "logicalcore.hpp"

namespace Business
{

LogicalCore::LogicalCore(std::vector<std::shared_ptr<AIBackendHandler> > &backends) :
    m_backendVector {backends}
{

}

void LogicalCore::processPrompt(const std::string &userPrompt)
{

}

void LogicalCore::interrupt()
{

}

}