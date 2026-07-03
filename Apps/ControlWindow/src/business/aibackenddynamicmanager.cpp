#include "aibackenddynamicmanager.hpp"

#include <Components/Logger/Logger.h>

AIBackendDynamicManager::AIBackendDynamicManager(QObject *parent) :
    QObject(parent)
{

}

bool AIBackendDynamicManager::isBackendOnline(const DBRecords::AIBackendInfo::id_t &id) const
{
    // TODO: Implement
    COMPLOG_DEBUG("isBackendOnline not implemented");
    return false;
}
