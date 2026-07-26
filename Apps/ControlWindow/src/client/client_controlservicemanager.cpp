#include "client_controlservicemanager.hpp"

#include <ProjectAgency/Exchange/HTTP.h>

#include <Components/Logger/Logger.h>

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
