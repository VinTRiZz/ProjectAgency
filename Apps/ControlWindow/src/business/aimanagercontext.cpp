#include "aimanagercontext.hpp"

#include "aibackendservicemanager.hpp"
#include "aibackenddynamicmanager.hpp"

AIManagerContext::AIManagerContext(QObject* parent) :
    QObject(parent)
{
    m_pBackendServiceManager = new AIBackendServiceManager(this);

    // DEBUG
    // m_pBackendServiceManager->setDebugEnabled(true);
    // m_pBackendServiceManager->updateBackends();

    m_pBackendDynamicManager = new AIBackendDynamicManager(this);
}

void AIManagerContext::setAddress(const QString &addr)
{
    m_pBackendServiceManager->setAddress(addr);
    m_pBackendDynamicManager->setAddress(addr);

    // Harvest data from a manager
    m_pBackendServiceManager->updateBackends();
}

QString AIManagerContext::getAddress() const
{
    return m_pBackendServiceManager->getAddress();
}

AIBackendServiceManager *AIManagerContext::getBackendServiceManager() const
{
    return m_pBackendServiceManager;
}

AIBackendDynamicManager *AIManagerContext::getBackendDynamicManager() const
{
    return m_pBackendDynamicManager;
}
