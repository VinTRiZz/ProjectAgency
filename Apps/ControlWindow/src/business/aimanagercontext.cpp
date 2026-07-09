#include "aimanagercontext.hpp"

#include "aibackendservicemanager.hpp"
#include "aibackenddynamicmanager.hpp"

AIManagerContext::AIManagerContext(QObject* parent) :
    QObject(parent)
{
    m_pBackendServiceManager = new AIBackendServiceManager(this);
    m_pBackendServiceManager->setDebugEnabled(true);
    m_pBackendServiceManager->updateBackends();

    m_pBackendDynamicManager = new AIBackendDynamicManager(this);
}

AIBackendServiceManager *AIManagerContext::getBackendServiceManager() const
{
    return m_pBackendServiceManager;
}

AIBackendDynamicManager *AIManagerContext::getBackendDynamicManager() const
{
    return m_pBackendDynamicManager;
}
