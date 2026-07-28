#include "controlservicemanager.hpp"

#include <Components/Logger/Logger.h>

ControlServiceManager::ControlServiceManager(QObject *parent)
    : QObject{parent}
{
    m_controlClient = new Client_ControlServiceManager(this);

    connect(m_controlClient, &QtCustom::Web::HTTPClientBase::sig_errorOccurs,
            this, &ControlServiceManager::sig_errorOccurs);
    connect(m_controlClient, &QtCustom::Web::HTTPClientBase::sig_validAddressSet,
            this, &ControlServiceManager::sig_connected);

    // m_controlClient.requrestGetDBParameters();
}

void ControlServiceManager::setAddress(const QString &addr)
{
    m_controlClient->setServer(addr);

    m_controlClient->init();
}

QString ControlServiceManager::getAddress() const
{
    return m_controlClient->getServer();
}

Client_ControlServiceManager *ControlServiceManager::getControlClient() const
{
    return m_controlClient;
}
