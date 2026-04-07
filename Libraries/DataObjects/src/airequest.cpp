#include "airequest.hpp"

#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>

namespace DataObjects {

std::string AIRequest::toJson() const
{
    // Prepare JSON
    nlohmann::json reqJson;
    reqJson["model"]    = m_modelName;
    reqJson["prompt"]   = m_request;
    reqJson["stream"]   = m_isStreamingEnabled;
    return reqJson.dump();
}

bool AIRequest::readJson(const std::string &iString)
{
    return false;
}

void AIRequest::setRequest(const std::string &request)
{
    m_request = request;

    // Prepare request
    boost::algorithm::replace_all(m_request, "\"", "\\\"");
    boost::algorithm::replace_all(m_request, "\n", "\\n");
}

void AIRequest::setModel(const std::string &modelName)
{
    m_modelName = modelName;
}

void AIRequest::setStreamingEnabled(bool isEn)
{
    m_isStreamingEnabled = isEn;
}

} // namespace DataObjects
