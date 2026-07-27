#include "controlservicemanager.hpp"

ControlServiceManager::ControlServiceManager(QObject *parent)
    : QObject{parent}
{
    connect(&m_controlClient, &QtCustom::Web::HTTPClientBase::sig_errorOccurs,
            this, &ControlServiceManager::sig_errorOccurs);

    // m_controlClient.requrestGetDBParameters();
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

void ControlServiceManager::setDatabaseConfiguration(const Exchange::DatabaseConfiguration &dbConfig)
{
    m_controlClient.requrestSetDBParameters(dbConfig);
}

Exchange::DatabaseConfiguration ControlServiceManager::getDatabaseConfiguration()
{
    auto dbParameters = m_controlClient.getDBParameters();
    if (dbParameters.first) {
        COMPLOG_WARNING("Requested DB parameters before update");
    }
    return dbParameters.second;
}
