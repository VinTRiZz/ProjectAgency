#pragma once

#include <Components/CustomQt/Web/HTTPClientBase.h>

#include <ProjectAgency/Exchange/EncryptedExchangeMaster.h>

class Client_ControlServiceManager : public QtCustom::Web::HTTPClientBase
{
    Q_OBJECT
public:
    explicit Client_ControlServiceManager(QObject *parent = nullptr);

    void init();

    Exchange::EncryptedExchangeMaster& getExchangeManager();

private:
    Exchange::EncryptedExchangeMaster m_exchangeManager;
    std::string m_pubkey;

    void processKeyExchange(const QString& responsePayload);
};
