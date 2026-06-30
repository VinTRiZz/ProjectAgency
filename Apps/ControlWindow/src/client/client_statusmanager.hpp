#pragma once

#include "client_base/httpclientbase.hpp"

#include <ProjectAgency/DeviceStatus.h>
#include <ProjectAgency/Exchange/HTTP.h>

#include <QTimer>

class Client_StatusManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit Client_StatusManager(QObject *parent = nullptr);

    /**
     * @brief setStatusTarget   Set base URL of server to ask for status
     * @param statusTarget      See Exchange::HTTP constants or constructed one
     */
    void setStatusTarget(const QString& statusTarget);

    /**
     * @brief start Starts requesting every 1 second status of server
     */
    void start();

    /**
     * @brief stop  Stop requesting status of server
     */
    void stop();

signals:
    void sig_gotStatus(const DataObjects::DeviceStatus& devStatus);

public slots:
    void requestStatus();

private:
    QTimer m_requestTimer;
    QString m_statusTarget {Exchange::HTTPv1::SERVER_STATUS.c_str()};
};
