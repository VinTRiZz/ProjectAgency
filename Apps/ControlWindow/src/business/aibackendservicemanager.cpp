#include "aibackendservicemanager.hpp"

#include <Components/Logger/Logger.h>

#include <ProjectAgency/DB/TEST/BackendInfoGenerator.h>

#include "client/client_backendservicemanager.hpp"

AIBackendServiceManager::AIBackendServiceManager(QObject *parent) :
    QObject{parent},
    m_backends {new BackendArrayHdl::value_t}
{
    m_pBackendServiceManager = new Client_BackendServiceManager(this);

    connect(this, &AIBackendServiceManager::sig_errorOccurs, [](const auto& errText){
        COMPLOG_ERROR("AIBackendServiceManager (exchange):", errText.toStdString());
    });

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
            COMPLOG_WARNING("AIBackendServiceManager: Existing backend added");
            return;
        }
        m_backends->insert(pBackendInfo);
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
            emit sig_backendUpdated(pBackend);
            return;
        }
        m_backends->insert(pBackendInfo);
        emit sig_backendAdded(pBackendInfo);
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
            m_backends->erase(pBackend);
        }
        emit sig_backendRemoved(pBackend);
    });


}

AIBackendServiceManager::~AIBackendServiceManager()
{
    delete m_backends.get();
}

void AIBackendServiceManager::setDebugEnabled(bool enableDebugMode)
{
    m_enableDebugMode = enableDebugMode;
}

void AIBackendServiceManager::setAddress(const QString &addr)
{
    m_pBackendServiceManager->setServer(addr);
}

QString AIBackendServiceManager::getAddress() const
{
    return m_pBackendServiceManager->getServer();
}

void AIBackendServiceManager::updateBackends()
{
    if (m_enableDebugMode) {
        AITest::BackendInfoGenerator gen;
        gen.setSeed(150);
        auto testRecords = gen.generate(10);
        for (auto trec : testRecords) {
            m_backends->insert(std::make_shared<DBRecords::AIBackendInfo>(trec));
        }
        for (const auto& bck : *m_backends) {
            emit sig_backendAdded(bck);
        }
    } else {
        m_pBackendServiceManager->requestIdList();
    }
}

Client_BackendServiceManager *AIBackendServiceManager::getClient() const
{
    return m_pBackendServiceManager;
}

DBRecords::AIBackendInfoPtr AIBackendServiceManager::getBackend(const DBRecords::AIBackendInfo::id_t &id) const
{
    auto lbnd = std::lower_bound(m_backends->begin(), m_backends->end(), id, [](const auto& pLeft, const auto& id){
        return (pLeft->getId() < id);
    });
    if (m_backends->end() == lbnd) {
        return {};
    }
    return ((*lbnd)->getId() == id ? (*lbnd) : DBRecords::AIBackendInfoPtr{});
}

BackendArrayHdl AIBackendServiceManager::getAllBackends() const
{
    return m_backends;
}
