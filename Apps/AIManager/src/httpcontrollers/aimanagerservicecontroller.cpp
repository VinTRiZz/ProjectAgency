#include "aimanagerservicecontroller.hpp"

#include <nlohmann/json.hpp>

#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Ecosystem/Utility.h>
#include <Components/Encryption/Encoding.h>
#include <Components/Logger/Logger.h>

#include <ProjectAgency/Exchange/ObjectSetting.h>

#include "common/settings.hpp"

AIManagerServiceController::AIManagerServiceController(ApplicationCore &appCore, AIManager &aiManager) :
    drogon::HttpController<AIManagerServiceController, false>(),
    ControllerBase(),
    m_appCore {appCore},
    m_aiManager {aiManager}
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

        // Test encrypting
        {
            if (!m_encMaster.setEncryptionKey(req->getBody().data())) {
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
    Exchange::ObjectSetting appSetting;
    if (!appSetting.readJson(settingJson)) {
        COMPLOG_WARNING("Failed to parse settings object:", appSetting.getError().what());
        return false;
    }
    auto decrValue = m_encMaster.decrypt(appSetting.m_value);
    if (!decrValue.has_value()) {
        COMPLOG_WARNING("Failed to set app setting [", appSetting.m_name, "] : ", m_encMaster.getError());
        return false;
    }

    if (appSetting.m_name == Exchange::HTTPv1::AIManagerSettingName::TOKEN) {
        m_appCore.setToken(decrValue.value());
        return true;
    }

    if (appSetting.m_name == Exchange::HTTPv1::AIManagerSettingName::API_PORT) {
        auto& settings = Common::ApplicationSettings::getInstance();
        auto pSett = settings.getSetting(Settings::SECTION_SYSTEM, Settings::SYSTEM_API_PORT);
        try {
            pSett->setValue(std::stoi(decrValue.value()));
        } catch (const std::invalid_argument& ex) {
            COMPLOG_WARNING("Failed to set app API port (invalid value)");
            return false;
        }
        settings.saveSettings();
        return true;
    }

    if (appSetting.m_name == Exchange::HTTPv1::AIManagerSettingName::INPUT_MODEL) {
        m_aiManager.setInputModel(decrValue.value());
        return true;
    }

    if (appSetting.m_name == Exchange::HTTPv1::AIManagerSettingName::DB_CONFIG) {
        Exchange::DatabaseConfiguration dbConfig;
        if (!dbConfig.readJson(decrValue.value())) {
            COMPLOG_WARNING("Failed to set app DB configuration:", dbConfig.getError().what());
            return false;
        }
        m_appCore.setDatabaseConfiguration(dbConfig);
        return true;
    }

    COMPLOG_WARNING("Unknown setting to set:", appSetting.m_name);
    return false;
}

void AIManagerServiceController::processServerGetSetting(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const std::string &settingName)
{
    if (!m_encMaster.canEncrypt()) {
        sendTextMessage(drogon::k401Unauthorized, "Key exchange failed", std::move(callback));
        return;
    }

    Exchange::ObjectSetting objSett;
    objSett.m_name = Encryption::decodeHex(settingName);

    if (objSett.m_name == Exchange::HTTPv1::AIManagerSettingName::TOKEN) {
        objSett.m_value = m_aiManager.getToken();
    } else if (objSett.m_name == Exchange::HTTPv1::AIManagerSettingName::INPUT_MODEL) {
        objSett.m_value = m_aiManager.getInputModel();
    } else if (objSett.m_name == Exchange::HTTPv1::AIManagerSettingName::API_PORT) {
        objSett.m_value = std::to_string(m_appCore.getPort());
    } else if (objSett.m_name == Exchange::HTTPv1::AIManagerSettingName::DB_CONFIG) {
        objSett.m_value = m_appCore.getDatabaseConfiguration().toJson();
    } else {
        sendTextMessage(drogon::k404NotFound, std::string("Invalid setting to get: ") + objSett.m_name, std::move(callback));
        return;
    }

    auto res = objSett.toJson();
    auto resEnc =  m_encMaster.encrypt(res);
    if (!resEnc.has_value()) {
        COMPLOG_WARNING("Encryption failure:", m_encMaster.getError().getDetailText());
        sendTextMessage(drogon::k500InternalServerError, "Encryption failure", std::move(callback));
        return;
    }
    sendTextMessage(drogon::k200OK, resEnc.value(), std::move(callback));
}
