#include "aimanagerservicecontroller.hpp"

#include <Components/Ecosystem/Utility.h>

AIManagerServiceController::AIManagerServiceController(ApplicationCore &appCore) :
    drogon::HttpController<AIManagerServiceController, false>(),
    ControllerBase(),
    m_appCore {appCore}
{

}

void AIManagerServiceController::processServerAction(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, int actionType)
{
    if (actionType == Exchange::HTTPv1::ServerAction::ActionStop) {
        sendTextMessage(drogon::k200OK, "Stopping in 3 seconds...", std::move(callback));

        // Stop in a time
        std::thread([this](){
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            m_appCore.stop();
        }).detach();
        return;
    }

    if (actionType == Exchange::HTTPv1::ServerAction::ActionRestart) {
        sendTextMessage(drogon::k200OK, "Restarting in 3 seconds...", std::move(callback));

        // Restart in a time
        std::thread ([this](){
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            Common::restartSelf();
        }).detach();
        return;
    }
}
