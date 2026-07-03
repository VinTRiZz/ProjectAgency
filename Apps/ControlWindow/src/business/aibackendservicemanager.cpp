#include "aibackendservicemanager.hpp"

#include <Components/Logger/Logger.h>

#include "client/client_backendservicemanager.hpp"

AIBackendServiceManager::AIBackendServiceManager(QObject *parent)
    : QObject{parent}
{
    m_pBackendServiceManager = new Client_BackendServiceManager(this);

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseIdList,
            this, [this](bool isSucceed, const auto& errorMsg, const auto& ids){
        if (!isSucceed) {
            emit sig_errorOccurs(errorMsg);
            return;
        }
        for (auto& id : ids) {
            m_pBackendServiceManager->requestConfigGet(id);
        }
    });

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseConfigAdd,
            this, [this](bool isSucceed, const auto& errorMsg, const auto& pBackendInfo){
        if (!isSucceed) {
            emit sig_errorOccurs(errorMsg);
            return;
        }
        auto pBackend = getBackend(pBackendInfo->getId());
        if (pBackend) {
            *pBackend = *pBackendInfo;
        } else {
            m_backends.insert(pBackendInfo);
        }
        emit sig_backendAdded(pBackend);
    });

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseConfigGet,
            this, [this](bool isSucceed, const auto& errorMsg, const auto& pBackendInfo){
        if (!isSucceed) {
            emit sig_errorOccurs(errorMsg);
            return;
        }
        auto pBackend = getBackend(pBackendInfo->getId());
        if (pBackend) {
            *pBackend = *pBackendInfo;
        } else {
            m_backends.insert(pBackendInfo);
        }
        emit sig_backendUpdated(pBackend);
    });

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseConfigSet,
            this, [this](bool isSucceed, const auto& errorMsg, const auto& pBackendInfo){
        if (!isSucceed) {
            emit sig_errorOccurs(errorMsg);
            return;
        }
        auto pBackend = getBackend(pBackendInfo->getId());
        if (!pBackend) {
            COMPLOG_ERROR("Unexpected error: set value of undefined backend");
            return;
        }
        *pBackend = *pBackendInfo;
        emit sig_backendUpdated(pBackend);
    });

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseConfigRemove,
            this, [this](bool isSucceed, const auto& errorMsg, const auto& backendId){
        if (!isSucceed) {
            emit sig_errorOccurs(errorMsg);
            return;
        }
        auto pBackend = getBackend(backendId);
        if (pBackend) {
            m_backends.erase(pBackend);
        }
        emit sig_backendRemoved(pBackend);
    });


}

void AIBackendServiceManager::updateBackends()
{
    m_pBackendServiceManager->requestIdList();
}

Client_BackendServiceManager *AIBackendServiceManager::getClient() const
{
    return m_pBackendServiceManager;
}

DBRecords::AIBackendInfoPtr AIBackendServiceManager::getBackend(const DBRecords::AIBackendInfo::id_t &id) const
{
    auto lbnd = std::lower_bound(m_backends.begin(), m_backends.end(), id, [](const auto& pLeft, const auto& id){
        return (pLeft->getId() < id);
    });
    if (m_backends.end() == lbnd) {
        return {};
    }
    return ((*lbnd)->getId() == id ? (*lbnd) : DBRecords::AIBackendInfoPtr{});
}

const std::set<DBRecords::AIBackendInfoPtr, AIBackendServiceManager::BackendLess> &AIBackendServiceManager::getAllBackends() const
{
    return m_backends;
}
