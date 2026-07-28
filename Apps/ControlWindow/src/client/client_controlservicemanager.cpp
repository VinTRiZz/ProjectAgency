#include "client_controlservicemanager.hpp"

#include <ProjectAgency/Exchange/HTTP.h>
#include <ProjectAgency/Exchange/ObjectSetting.h>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

#include <nlohmann/json.hpp>

#include <QRegularExpression>

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

        const auto setSettingPath =
            QString::fromStdString(Exchange::HTTPv1::QT_SERVER_ACTION).arg(
                QString::number(Exchange::HTTPv1::AIMA_SetSetting));
        if (reqPath == setSettingPath) {
            // Ignore
            return;
        }

        auto getSettingPathBase = QString::fromStdString(Exchange::HTTPv1::QT_SERVER_GET_SETTING);
        const auto getSettingPathRegexp = QRegularExpression(getSettingPathBase.arg("(.*)"));
        const auto matches = getSettingPathRegexp.match(reqPath);
        if (matches.hasMatch()) {
            auto settingName = Encryption::decodeHex(matches.captured(1).toStdString());
            if (settingName.empty()) {
                COMPLOG_WARNING("ControlServiceManager: Failed to decrypt setting name");
                return;
            }
            auto decodedSettingValue = m_exchangeManager.decrypt(responsePayload.toStdString());
            if (!decodedSettingValue.has_value()) {
                COMPLOG_WARNING("ControlServiceManager: Failed to decode setting:", m_exchangeManager.getError().what());
                return;
            }
            Exchange::ObjectSetting sett;
            if (!sett.readJson(decodedSettingValue.value())) {
                COMPLOG_WARNING("ControlServiceManager: Failed to decode setting data:", sett.getError().what());
                return;
            }
            if (sett.m_name == Exchange::HTTPv1::AIManagerSettingName::TOKEN) {
                m_settingFuture_token.setValue(std::move(sett.m_value));
                return;
            }
            if (sett.m_name == Exchange::HTTPv1::AIManagerSettingName::API_PORT) {
                try {
                    m_settingFuture_port.setValue(std::stoi(sett.m_value));
                } catch (const std::invalid_argument& ex) {
                    COMPLOG_WARNING("ControlServiceManager: Failed to decode API port (invalid value)");
                    return;
                }
                return;
            }
            if (sett.m_name == Exchange::HTTPv1::AIManagerSettingName::INPUT_MODEL) {
                m_settingFuture_inputModel.setValue(std::move(sett.m_value));
                return;
            }
            if (sett.m_name == Exchange::HTTPv1::AIManagerSettingName::DB_CONFIG) {
                Exchange::DatabaseConfiguration dbConfig;
                if (!dbConfig.readJson(sett.m_value)) {
                    COMPLOG_WARNING("ControlServiceManager: Failed to decode db parameters:", dbConfig.getError().what());
                    return;
                }
                m_settingFuture_dbConfig.setValue(std::move(dbConfig));
                return;
            }
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
    requestSetSetting(Exchange::HTTPv1::AIManagerSettingName::TOKEN, tokenStr.toStdString());
}

const Client_ControlServiceManager::RequestedData<std::string>& Client_ControlServiceManager::getToken() const
{
    m_settingFuture_token.invalidate();
    requestGetSetting(Exchange::HTTPv1::AIManagerSettingName::TOKEN);
    return m_settingFuture_token;
}

void Client_ControlServiceManager::requrestSetPort(uint16_t port)
{
    requestSetSetting(Exchange::HTTPv1::AIManagerSettingName::API_PORT, std::to_string(port));
}

const Client_ControlServiceManager::RequestedData<uint16_t>& Client_ControlServiceManager::getPort() const
{
    m_settingFuture_port.invalidate();
    requestGetSetting(Exchange::HTTPv1::AIManagerSettingName::API_PORT);
    return m_settingFuture_port;
}

void Client_ControlServiceManager::requrestSetInputModel(const QString &modelStr)
{
    requestSetSetting(Exchange::HTTPv1::AIManagerSettingName::INPUT_MODEL, modelStr.toStdString());
}

const Client_ControlServiceManager::RequestedData<std::string>& Client_ControlServiceManager::getInputModel() const
{
    m_settingFuture_inputModel.invalidate();
    requestGetSetting(Exchange::HTTPv1::AIManagerSettingName::INPUT_MODEL);
    return m_settingFuture_inputModel;
}

void Client_ControlServiceManager::requrestSetDBParameters(const Exchange::DatabaseConfiguration &dbConfig)
{
    requestSetSetting(Exchange::HTTPv1::AIManagerSettingName::DB_CONFIG, dbConfig.toJson());
}

const Client_ControlServiceManager::RequestedData<Exchange::DatabaseConfiguration>& Client_ControlServiceManager::getDBConfig() const
{
    m_settingFuture_dbConfig.invalidate();
    requestGetSetting(Exchange::HTTPv1::AIManagerSettingName::DB_CONFIG);
    return m_settingFuture_dbConfig;
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

void Client_ControlServiceManager::requestGetSetting(const std::string &settingName) const
{
    if (m_pubkey.empty()) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::InterfaceEncInvalidPubkey, "Key exchange failed"));
        return;
    }
    const auto getSettingPath =
        QString::fromStdString(Exchange::HTTPv1::QT_SERVER_GET_SETTING).arg(
            QString::fromStdString(Encryption::encodeHex(settingName)));
    sendSimpleRequestGet(getSettingPath);
}
