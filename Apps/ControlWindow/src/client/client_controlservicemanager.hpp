#pragma once

#include <Components/CustomQt/Web/HTTPClientBase.h>

#include <ProjectAgency/Exchange/EncryptedExchangeMaster.h>
#include <ProjectAgency/Exchange/DatabaseConfiguration.h>

#include <future>

class Client_ControlServiceManager : public QtCustom::Web::HTTPClientBase
{
    Q_OBJECT
public:
    template <typename DataT>
    class RequestedData
    {
        friend class Client_ControlServiceManager;
    public:
        using value_t = DataT;

        void invalidate() {
            if (m_isValuePending) { m_selfValue.set_value({}); } // Server error handling
            m_selfValue = {};
            m_sharedFut = m_selfValue.get_future().share();
        }

        value_t operator*() const {
            return m_sharedFut.get();
        }

        value_t getValue() const {
            return m_sharedFut.get();
        }

    private:
        bool m_isValuePending {false};
        std::promise<value_t> m_selfValue;
        std::shared_future<value_t> m_sharedFut { m_selfValue.get_future().share() };

        void setValue(value_t&& iVal) {
            m_isValuePending = false;
            m_selfValue.set_value(std::move(iVal));
        }
    };


    explicit Client_ControlServiceManager(QObject *parent = nullptr);

    void init();

    void requrestSetToken(const QString& tokenStr);
    const RequestedData<std::string>& getToken() const;

    void requrestSetPort(uint16_t port);
    const RequestedData<uint16_t>& getPort() const;

    void requrestSetInputModel(const QString& modelStr);
    const RequestedData<std::string>& getInputModel() const;

    void requrestSetDBParameters(const Exchange::DatabaseConfiguration& dbConfig);
    const RequestedData<Exchange::DatabaseConfiguration>& getDBConfig() const;

    Exchange::EncryptedExchangeMaster& getExchangeManager();

private:
    Exchange::EncryptedExchangeMaster m_exchangeManager;
    std::string m_pubkey;

    // For requesting
    mutable RequestedData<std::string> m_settingFuture_token;
    mutable RequestedData<uint16_t>    m_settingFuture_port;
    mutable RequestedData<std::string> m_settingFuture_inputModel;
    mutable RequestedData<Exchange::DatabaseConfiguration> m_settingFuture_dbConfig;

    void processKeyExchange(const QString& responsePayload);
    void requestSetSetting(const std::string& settingName, const std::string& settingValue);
    void requestGetSetting(const std::string& settingName) const;
};
