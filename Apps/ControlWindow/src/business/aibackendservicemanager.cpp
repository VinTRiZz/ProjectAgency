#include "aibackendservicemanager.hpp"

#include <Components/Logger/Logger.h>

#include <ProjectAgency/DB/TEST/BackendInfoGenerator.h>

#include "client/client_backendservicemanager.hpp"

AIBackendServiceManager::AIBackendServiceManager(QObject *parent) :
    QObject{parent},
    m_backends {new BackendArrayHdl::value_t}
{
    m_pBackendServiceManager = new Client_BackendServiceManager(this);

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_errorOccurs,
            this, &AIBackendServiceManager::sig_errorOccurs);

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseIdList,
            this, [this](const auto& ids){
        for (auto& id : ids) {
            m_pBackendServiceManager->requestConfigGet(id);
        }
    });

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseConfigAdd,
            this, [this](const auto& pBackendInfo){
        if (!pBackendInfo->getId().has_value()) {
            return;
        }
        auto pBackend = getBackend(pBackendInfo->getId().value());
        if (pBackend) {
            COMPLOG_WARNING("AIBackendServiceManager: Existing backend added");
            return;
        }
        m_backends->insert(pBackendInfo);
        emit sig_backendAdded(pBackend);
    });

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseConfigGet,
            this, [this](const auto& pBackendInfo){
        if (!pBackendInfo->getId().has_value()) {
            return;
        }
        auto pBackend = getBackend(pBackendInfo->getId().value());
        if (pBackend) {
            *pBackend = *pBackendInfo;
            emit sig_backendUpdated(pBackend);
            return;
        }
        m_backends->insert(pBackendInfo);
        emit sig_backendAdded(pBackendInfo);
    });

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseConfigSet,
            this, [this](const auto& pBackendInfo){
        if (!pBackendInfo->getId().has_value()) {
            return;
        }
        auto pBackend = getBackend(pBackendInfo->getId().value());
        if (!pBackend) {
            COMPLOG_ERROR("Unexpected error: set value of undefined backend");
            return;
        }
        *pBackend = *pBackendInfo;
        emit sig_backendUpdated(pBackend);
    });

    connect(m_pBackendServiceManager, &Client_BackendServiceManager::sig_responseConfigRemove,
            this, [this](const auto& backendId){
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

DBRecords::AIBackendInfoPtr AIBackendServiceManager::getBackend(const DBRecords::AIBackendInfo::id_nullable_t &id) const
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

void AIBackendServiceManager::emitError(const QString &errText, const QString &errDetail)
{
    auto resDetail = (errDetail.isEmpty() ? errText : errText + "(" + errDetail + ")");
    emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::GuiServerProcessingFail, resDetail.toStdString()));
}
