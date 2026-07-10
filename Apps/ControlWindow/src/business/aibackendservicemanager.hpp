#pragma once

#include <QObject>

#include <set>

#include <ProjectAgency/DB/AIBackendInfo.h>

#include <Components/ExtraClasses/Containers/HandlerBase.h>

class Client_BackendServiceManager;

using BackendArray = std::set<DBRecords::AIBackendInfoPtr>;
using BackendArrayHdl = ExtraClasses::HandlerBase<BackendArray>;

/**
 * @brief The AIBackendServiceManager class Instance to work with backend info stated in AIManager
 * @note work only with static info (name, existance, ip, etc.)
 */
class AIBackendServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit AIBackendServiceManager(QObject *parent = nullptr);
    ~AIBackendServiceManager();

    void setDebugEnabled(bool enableDebugMode);

    void setAddress(const QString& addr);
    QString getAddress() const;

    void updateBackends();
    Client_BackendServiceManager* getClient() const;

    DBRecords::AIBackendInfoPtr getBackend(const DBRecords::AIBackendInfo::id_t& id) const;
    BackendArrayHdl getAllBackends() const;

signals:
    void sig_backendAdded(const DBRecords::AIBackendInfoPtr& pBackend);
    void sig_backendUpdated(const DBRecords::AIBackendInfoPtr& pBackend);
    void sig_backendRemoved(const DBRecords::AIBackendInfoPtr& backendId);

    void sig_errorOccurs(const QString& errorText, const QString& errDetail);

private:
    bool m_enableDebugMode {false}; // Allows generating test samples
    BackendArrayHdl m_backends;
    Client_BackendServiceManager* m_pBackendServiceManager {nullptr};
};
