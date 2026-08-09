#include "aibackenddynamicmanager.hpp"

#include "aibackendservicemanager.hpp"

#include <Components/Logger/Logger.h>

AIBackendDynamicManager::AIBackendDynamicManager(QObject *parent) :
    QObject(parent)
{

}

void AIBackendDynamicManager::setAddress(const QString &addr)
{
    // TODO: Implement
    emit sig_connected(); // to not break logic
}

QString AIBackendDynamicManager::getAddress() const
{
    // TODO: Implement
    return {};
}

void AIBackendDynamicManager::setServiceManager(AIBackendServiceManager *pManager)
{
    m_pServiceManager = pManager;
}

bool AIBackendDynamicManager::isBackendOnline(const DBRecords::AIBackendInfo::id_nullable_t &id) const
{
    // TODO: Implement
    return false;
}
