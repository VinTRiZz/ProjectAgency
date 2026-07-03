#include "aimanagercontext.hpp"

#include "aibackendservicemanager.hpp"
#include "aibackenddynamicmanager.hpp"

AIManagerContext::AIManagerContext(QObject* parent) :
    QObject(parent)
{
    m_pBackendServiceManager = new AIBackendServiceManager(this);

    m_pBackendDynamicManager = new AIBackendDynamicManager(this);
}

AIBackendDynamicManager *AIManagerContext::getBackendDynamicManager() const
{
    return m_pBackendDynamicManager;
}
