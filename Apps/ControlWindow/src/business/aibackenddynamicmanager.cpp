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
    emit sig_errorOccurs({Exchange::ErrorCode::SystemNotImplemented, "Can not set address of dynamic data"});
}

QString AIBackendDynamicManager::getAddress() const
{
    // TODO: Implement
    emit sig_errorOccurs(Exchange::ErrorCode::SystemNotImplemented);
    return {};
}

void AIBackendDynamicManager::setServiceManager(AIBackendServiceManager *pManager)
{
    m_pServiceManager = pManager;
}

bool AIBackendDynamicManager::isBackendOnline(const DBRecords::AIBackendInfo::id_nullable_t &id) const
{
    // TODO: Implement
    emit sig_errorOccurs(Exchange::ErrorCode::SystemNotImplemented);
    return false;
}
