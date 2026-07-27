#include "client_controlservicemanager.hpp"

#include <ProjectAgency/Exchange/HTTP.h>
#include <ProjectAgency/Exchange/ObjectSetting.h>

#include <Components/Logger/Logger.h>

#include <nlohmann/json.hpp>

Client_ControlServiceManager::Client_ControlServiceManager(QObject *parent)
    : QtCustom::Web::HTTPClientBase{parent}
{
    connect(this, &QtCustom::Web::HTTPClientBase::sig_simpleResponsePut,
            this, [this](const auto& reqPath, const auto& responsePayload){
        const auto keyExchangePath =
            QString::fromStdString(Exchange::HTTPv1::QT_SERVER_ACTION).arg(
                QString::number(Exchange::HTTPv1::AIMA_ExchangePublicKeys));
        if (reqPath == keyExchangePath) {
            processKeyExchange(responsePayload);
            return;
        }
    });

    m_exchangeManager.init();
}

void Client_ControlServiceManager::init()
{
    m_pubkey = {};
    const auto keyExchangePath =
        QString::fromStdString(Exchange::HTTPv1::QT_SERVER_ACTION).arg(
            QString::number(Exchange::HTTPv1::AIMA_ExchangePublicKeys));
    sendSimpleRequestPut(keyExchangePath, QString::fromStdString(m_exchangeManager.getPubkey()));
}

void Client_ControlServiceManager::requrestSetToken(const QString &tokenStr)
{
    requestSetSetting("token", tokenStr.toStdString());
}

void Client_ControlServiceManager::requrestSetPort(uint16_t port)
{
    requestSetSetting("API port", std::to_string(port));
}

void Client_ControlServiceManager::requrestSetModel(const QString &modelStr)
{
    requestSetSetting("input model", modelStr.toStdString());
}

void Client_ControlServiceManager::requrestSetDBParameters(const Exchange::DatabaseConfiguration &dbConfig)
{
    requestSetSetting("DB parameters", dbConfig.toJson());
}

Exchange::EncryptedExchangeMaster &Client_ControlServiceManager::getExchangeManager()
{
    return m_exchangeManager;
}

void Client_ControlServiceManager::processKeyExchange(const QString &responsePayload)
{
    m_pubkey = {};
    if (responsePayload.isEmpty()) {
        COMPLOG_ERROR("ControlServiceManager: Empty key for exchange");
        return;
    }
    m_pubkey = responsePayload.toStdString();
    COMPLOG_INFO("ControlServiceManager: key exchange complete");
}

void Client_ControlServiceManager::requestSetSetting(const std::string &settingName, const std::string &settingValue)
{
    const auto setSettingPath =
        QString::fromStdString(Exchange::HTTPv1::QT_SERVER_ACTION).arg(
            QString::number(Exchange::HTTPv1::AIMA_SetSetting));
    if (m_pubkey.empty()) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::InterfaceEncInvalidPubkey, "Key exchange failed"));
        return;
    }
    auto encryptedValue = m_exchangeManager.encrypt(settingValue, m_pubkey);
    if (!encryptedValue.has_value()) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::InterfaceEncInvalidPubkey, "Failed to encrypt message"));
        return;
    }

    Exchange::ObjectSetting sett;
    sett.m_name = settingName;
    sett.m_value = encryptedValue.value();

    sendSimpleRequestPut(setSettingPath, QString::fromStdString(sett.toJson()));
}
