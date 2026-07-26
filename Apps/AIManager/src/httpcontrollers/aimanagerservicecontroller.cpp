#include "aimanagerservicecontroller.hpp"

#include <nlohmann/json.hpp>

#include <Components/Ecosystem/Utility.h>
#include <Components/Logger/Logger.h>

AIManagerServiceController::AIManagerServiceController(ApplicationCore &appCore) :
    drogon::HttpController<AIManagerServiceController, false>(),
    ControllerBase(),
    m_appCore {appCore}
{
    m_encMaster.init();
}

void AIManagerServiceController::processServerAction(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, int actionType)
{
    switch (actionType)
    {
    case Exchange::HTTPv1::AIManagerAction::AIMA_Stop:
        sendTextMessage(drogon::k200OK, "Stopping in 3 seconds...", std::move(callback));

        // Stop in a time
        std::thread([this](){
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            m_appCore.stop();
        }).detach();
        break;

    case Exchange::HTTPv1::AIManagerAction::AIMA_Restart:
        sendTextMessage(drogon::k200OK, "Restarting in 3 seconds...", std::move(callback));

        // Restart in a time
        std::thread ([this](){
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            Common::restartSelf();
        }).detach();
        break;

    case Exchange::HTTPv1::AIManagerAction::AIMA_ExchangePublicKeys:
        m_sessionPubkey = req->getBody();

        // Test encrypting
        {
            std::string testStr {"Examples string for test"};
            auto testEnc = m_encMaster.encrypt(testStr, m_sessionPubkey);
            if (!testEnc.has_value()) {
                m_sessionPubkey = {};
                sendTextMessage(drogon::k400BadRequest, "Invalid session key", std::move(callback));
                break;
            }
        }
        sendTextMessage(drogon::k200OK, m_encMaster.getPubkey(), std::move(callback));
        break;

    case Exchange::HTTPv1::AIManagerAction::AIMA_SetSetting:
        if (!setAppSetting(req->getBody().data())) {
            sendTextMessage(drogon::k406NotAcceptable, {}, std::move(callback));
        } else {
            sendTextMessage(drogon::k200OK, {}, std::move(callback));
        }
        break;
    }
}

bool AIManagerServiceController::setAppSetting(const std::string &settingJson)
{
    try {
        auto js = nlohmann::json::parse(settingJson);
        if (!js.contains("name") || !js.contains("value")) {
            COMPLOG_WARNING("Failed to set app setting (invalid input json)");
            return false;
        }
        if (js["name"] == "token") {
            auto decrToken = m_encMaster.decrypt(js["value"]);
            if (!decrToken.has_value()) {
                COMPLOG_WARNING("Failed to set app token:", m_encMaster.getError());
                return false;
            }
            m_appCore.setToken(decrToken.value());
            return true;
        }
    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_WARNING("Failed to set app setting:", ex.what());
    }
    return false;
}
