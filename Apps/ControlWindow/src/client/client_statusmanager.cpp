#include "client_statusmanager.hpp"

#include <QNetworkReply>

Client_StatusManager::Client_StatusManager(QObject *parent)
    : HTTPClientBase{parent}
{
    connect(&m_requestTimer, &QTimer::timeout,
            this, &Client_StatusManager::requestStatus);
}

void Client_StatusManager::setStatusTarget(const QString &statusTarget)
{
    m_statusTarget = statusTarget;
}

void Client_StatusManager::start()
{
    m_requestTimer.start(1000);
}

void Client_StatusManager::stop()
{
    m_requestTimer.stop();
}

void Client_StatusManager::requestStatus()
{
    auto& requester = getRequester();
    auto req = createRequest(m_statusTarget); // TODO: Invalidates after request?
    auto resp = requester.get(req);
    connect(resp, &QNetworkReply::finished,
            this, [this, resp](){
        if (resp->error() != QNetworkReply::NoError) {
            return;
        }
        DataObjects::DeviceStatus status;
        if (!status.readJson(resp->readAll().toStdString())) {
            return;
        }
        emit sig_gotStatus(status);
    });
}
