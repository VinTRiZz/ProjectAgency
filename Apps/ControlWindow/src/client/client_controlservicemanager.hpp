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
    public:
        using value_t = DataT;

        ~RequestedData() { invalidate(); }

        void invalidate() {
            if (isPending()) { // Server error handling
                m_selfValue.set_value({});
            }
            m_selfValue = {};
            m_sharedFut = m_selfValue.get_future().share();
        }

        void setValue(value_t&& iVal) {
            m_selfValue.set_value(std::move(iVal));
        }

        std::optional<value_t> getValue(const uint16_t timeoutMs = 0) const {
            if (!isPending()) return {};
            auto futureStatus = m_sharedFut.wait_for(std::chrono::milliseconds(timeoutMs));
            if (std::future_status::ready != futureStatus) {
                return {};
            }
            return m_sharedFut.get();
        }

        bool isPending() const {
            auto futureStatus = m_sharedFut.wait_for(std::chrono::milliseconds(0));
            return (std::future_status::ready != futureStatus);
        }

    private:
        std::promise<value_t> m_selfValue;
        std::shared_future<value_t> m_sharedFut { m_selfValue.get_future().share() };
    };


    explicit Client_ControlServiceManager(QObject *parent = nullptr);
    ~Client_ControlServiceManager();

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

signals:
    void sig_keyExchangeComplete();

private:
    Exchange::EncryptedExchangeMaster m_exchangeManager;
    std::string m_pubkey;

    // For requesting
    mutable RequestedData<std::string> m_settingFuture_token;
    mutable RequestedData<uint16_t>    m_settingFuture_port;
    mutable RequestedData<std::string> m_settingFuture_inputModel;
    mutable RequestedData<Exchange::DatabaseConfiguration> m_settingFuture_dbConfig;

    void invalidateFutures();

    void processKeyExchange(const QString& responsePayload);
    void requestSetSetting(const std::string& settingName, const std::string& settingValue);
    bool requestGetSetting(const std::string& settingName) const;
};
