#pragma once

#include <QObject>

#include "client/client_controlservicemanager.hpp"

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

signals:
    void sig_errorOccurs(const Exchange::Error& err);

private:
    Client_ControlServiceManager m_controlClient;
};
