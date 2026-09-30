#pragma once

#include <Components/CustomQt/Web/HTTPClientBase.h>

#include <ProjectAgency/Exchange/EncryptedExchangeMaster.h>
#include <ProjectAgency/Exchange/DatabaseConfiguration.h>

class Client_ControlServiceManager : public QtCustom::Web::HTTPClientBase
{
    Q_OBJECT
public:
    explicit Client_ControlServiceManager(QObject *parent = nullptr);
    ~Client_ControlServiceManager();

    void init();

    void requestConfiguration();

    void requrestSetToken(const QString& tokenStr);
    std::string getToken() const;

    void requrestSetPort(uint16_t port);
    uint16_t getPort() const;

    void requrestSetInputModel(const QString& modelStr);
    std::string getInputModel() const;

    void requrestSetDBParameters(const Exchange::DatabaseConfiguration& dbConfig);
    Exchange::DatabaseConfiguration getDBConfig() const;

    Exchange::EncryptedExchangeMaster& getExchangeManager();

signals:
    void sig_keyExchangeComplete();
    void sig_configChanged();

private:
    Exchange::EncryptedExchangeMaster m_exchangeManager;

    // For requesting
    std::string m_settingFuture_token;
    uint16_t    m_settingFuture_port;
    std::string m_settingFuture_inputModel;
    Exchange::DatabaseConfiguration m_settingFuture_dbConfig;

    void processKeyExchange(const QString& responsePayload);
    void requestSetSetting(const std::string& settingName, const std::string& settingValue);
    bool requestGetSetting(const std::string& settingName) const;
};
