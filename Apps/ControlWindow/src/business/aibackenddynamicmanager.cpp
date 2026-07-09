#include "aibackenddynamicmanager.hpp"

#include "aibackendservicemanager.hpp"

#include <Components/Logger/Logger.h>

AIBackendDynamicManager::AIBackendDynamicManager(QObject *parent) :
    QObject(parent)
{

}

void AIBackendDynamicManager::setServiceManager(AIBackendServiceManager *pManager)
{
    m_pServiceManager = pManager;
}

bool AIBackendDynamicManager::isBackendOnline(const DBRecords::AIBackendInfo::id_t &id) const
{
    // TODO: Implement
    COMPLOG_DEBUG("isBackendOnline not implemented");
    return false;
}
