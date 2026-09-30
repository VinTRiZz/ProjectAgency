#pragma once

#include "controllerbase.hpp"

#include <ProjectAgency/Exchange/HTTP.h>
#include <ProjectAgency/Exchange/EncryptedExchangeMaster.h>

#include "business/applicationcore.hpp"
#include "business/aimanager.hpp"

class AIManagerServiceController : public drogon::HttpController<AIManagerServiceController, false>,
                                   public ControllerBase
{
public:
    AIManagerServiceController(ApplicationCore& appCore, AIManager& aiManager);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(
            AIManagerServiceController::processServerAction,
            Exchange::HTTPv1::SERVER_ACTION,
            drogon::Put);

        ADD_METHOD_TO(
            AIManagerServiceController::processServerGetSetting,
            Exchange::HTTPv1::SERVER_GET_SETTING,
            drogon::Get);
    METHOD_LIST_END

    void processServerAction(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback,
        int actionType);

    void processServerGetSetting(
        const drogon::HttpRequestPtr &req,
        ResponseCallback_t &&callback,
        const std::string& settingName);

private:
    ApplicationCore& m_appCore;
    AIManager& m_aiManager;

    Exchange::EncryptedExchangeMaster m_encMaster;

    bool setAppSetting(const std::string& settingJson);
};
