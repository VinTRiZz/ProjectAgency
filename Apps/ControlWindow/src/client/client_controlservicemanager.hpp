#pragma once

#include <Components/CustomQt/Web/HTTPClientBase.h>

#include <ProjectAgency/Exchange/EncryptedExchangeMaster.h>
#include <ProjectAgency/Exchange/DatabaseConfiguration.h>

class Client_ControlServiceManager : public QtCustom::Web::HTTPClientBase
{
    Q_OBJECT
public:
    explicit Client_ControlServiceManager(QObject *parent = nullptr);

    void init();

    void requrestSetToken(const QString& tokenStr);
    void requrestSetPort(uint16_t port);
    void requrestSetModel(const QString& modelStr);
    void requrestSetDBParameters(const Exchange::DatabaseConfiguration& dbConfig);

    Exchange::EncryptedExchangeMaster& getExchangeManager();

private:
    Exchange::EncryptedExchangeMaster m_exchangeManager;
    std::string m_pubkey;

    void processKeyExchange(const QString& responsePayload);
};
