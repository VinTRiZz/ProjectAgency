#pragma once

#include <QObject>

#include "client/client_controlservicemanager.hpp"

#include <ProjectAgency/Exchange/DatabaseConfiguration.h>

namespace Exchange {
class Error;
}

class ControlServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit ControlServiceManager(QObject *parent = nullptr);

    void setAddress(const QString& addr);
    QString getAddress() const;

    Client_ControlServiceManager* getControlClient() const;

signals:
    void sig_connected() const;
    void sig_errorOccurs(const Exchange::Error& err);

private:
    Client_ControlServiceManager* m_controlClient {nullptr};
};
