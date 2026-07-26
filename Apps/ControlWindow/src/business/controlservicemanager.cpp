#include "controlservicemanager.hpp"

ControlServiceManager::ControlServiceManager(QObject *parent)
    : QObject{parent}
{
    connect(&m_controlClient, &QtCustom::Web::HTTPClientBase::sig_errorOccurs,
            this, &ControlServiceManager::sig_errorOccurs);
}

void ControlServiceManager::setAddress(const QString &addr)
{
    m_controlClient.setServer(addr);

    m_controlClient.init();
}

QString ControlServiceManager::getAddress() const
{
    return m_controlClient.getServer();
}
