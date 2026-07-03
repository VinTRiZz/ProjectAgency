#pragma once

#include <QObject>

#include <set>

#include <ProjectAgency/DB/AIBackendInfo.h>

class Client_BackendServiceManager;

/**
 * @brief The AIBackendServiceManager class Instance to work with backend info stated in AIManager
 * @note work only with static info (name, existance, ip, etc.)
 */
class AIBackendServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit AIBackendServiceManager(QObject *parent = nullptr);

    void updateBackends();
    Client_BackendServiceManager* getClient() const;

    // Sorter for std::set
    struct BackendLess
    {
        bool operator()(const DBRecords::AIBackendInfoPtr& pLeft,
                        const DBRecords::AIBackendInfoPtr& pRight) const {
            return ((!pLeft && pRight) ||
                    (pLeft && pRight && (pLeft->getId() < pRight->getId())));
        }
    };

    DBRecords::AIBackendInfoPtr getBackend(const DBRecords::AIBackendInfo::id_t& id) const;
    const std::set<DBRecords::AIBackendInfoPtr, BackendLess>& getAllBackends() const;

signals:
    void sig_backendAdded(const DBRecords::AIBackendInfoPtr& pBackend);
    void sig_backendUpdated(const DBRecords::AIBackendInfoPtr& pBackend);
    void sig_backendRemoved(const DBRecords::AIBackendInfoPtr& backendId);

    void sig_errorOccurs(const QString& errorText);

private:
    std::set<DBRecords::AIBackendInfoPtr, BackendLess> m_backends;
    Client_BackendServiceManager* m_pBackendServiceManager {nullptr};
};
