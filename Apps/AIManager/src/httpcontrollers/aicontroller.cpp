#include "aicontroller.hpp"

AIController::AIController(AIManager &aiMan) :
    m_aiManager {aiMan}
{

}

void AIController::processGetBackends(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{

}

void AIController::processGetBackendConfig(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{

}

void AIController::processSetBackendConfig(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{

}

void AIController::processStartDevelop(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{

}

void AIController::processGetDevelopStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{

}

void AIController::processStopDevelop(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{

}
