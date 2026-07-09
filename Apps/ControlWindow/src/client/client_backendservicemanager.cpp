#include "client_backendservicemanager.hpp"

#include <QNetworkReply>
#include <QNetworkRequest>

#include <ProjectAgency/Exchange/HTTP.h>

#include <Components/Logger/Logger.h>

#include <nlohmann/json.hpp>

Client_BackendServiceManager::Client_BackendServiceManager(QObject *parent)
    : HTTPClientBase{parent}
{

}

void Client_BackendServiceManager::requestIdList()
{
    auto& requester = getRequester();
    auto req = createRequest(Exchange::HTTPv1::BACKENDS_ID_LIST.c_str());
    auto resp = requester.get(req);
    connect(resp, &QNetworkReply::finished,
            this, [this, resp](){
                if (resp->error() != QNetworkReply::NoError) {
                    auto errText = resp->readAll();
                    COMPLOG_WARNING("Backend ID list error response:", errText.toStdString());
                    emit sig_responseIdList(false, errText);
                    return;
                }
                try {
                    auto responseJson = nlohmann::json::parse(resp->readAll().toStdString());
                    std::vector<DBRecords::AIBackendInfo::id_t> ids;
                    for (auto& js : responseJson) {
                        ids.push_back(js);
                    }
                    emit sig_responseIdList(true, {}, ids);
                } catch (nlohmann::json::exception& ex) {
                    COMPLOG_ERROR("Backend ID list parsing error:", ex.what());
                    emit sig_responseIdList(false, "Response parsing error");
                    return;
                }
            });
}

void Client_BackendServiceManager::requestConfigAdd(const DBRecords::AIBackendInfoPtr &pBackendInfo)
{
    if (!pBackendInfo) {
        COMPLOG_WARNING("Client_BackendServiceManager::requestConfigAdd: Skipped invalid backend info");
        return;
    }
    auto& requester = getRequester();
    auto req = createRequest(createTarget(Exchange::HTTPv1::QT_BACKEND_CONFIG_ADD, pBackendInfo));
    auto resp = requester.get(req);
    connect(resp, &QNetworkReply::finished,
            this, [this, resp, pBackendInfo](){
                if (resp->error() != QNetworkReply::NoError) {
                    auto errText = resp->readAll();
                    COMPLOG_WARNING("Backend config add error response:", errText.toStdString());
                    emit sig_responseConfigAdd(false, errText);
                    return;
                }
                pBackendInfo->readJson(resp->readAll().toStdString()); // Apply updates, including ID set
                emit sig_responseConfigAdd(true, {}, pBackendInfo);
            });
}

void Client_BackendServiceManager::requestConfigGet(const DBRecords::AIBackendInfo::id_t &backendId)
{
    auto& requester = getRequester();
    auto req = createRequest(createTarget(Exchange::HTTPv1::QT_BACKEND_CONFIG_GET, backendId));
    auto resp = requester.get(req);
    connect(resp, &QNetworkReply::finished,
            this, [this, resp](){
                if (resp->error() != QNetworkReply::NoError) {
                    auto errText = resp->readAll();
                    COMPLOG_WARNING("Backend config get error response:", errText.toStdString());
                    emit sig_responseConfigGet(false, errText);
                    return;
                }
                DBRecords::AIBackendInfo pBackendInfo;
                pBackendInfo.readJson(resp->readAll().toStdString());
                emit sig_responseConfigGet(true, {}, pBackendInfo.toPointer());
            });
}

void Client_BackendServiceManager::requestConfigSet(const DBRecords::AIBackendInfoPtr &pBackendInfo)
{
    if (!pBackendInfo) {
        COMPLOG_WARNING("Client_BackendServiceManager::requestConfigSet: Skipped invalid backend info");
        return;
    }
    auto& requester = getRequester();
    auto req = createRequest(createTarget(Exchange::HTTPv1::BACKEND_CONFIG_SET, pBackendInfo));
    auto resp = requester.get(req);
    connect(resp, &QNetworkReply::finished,
            this, [this, resp](){
                if (resp->error() != QNetworkReply::NoError) {
                    auto errText = resp->readAll();
                    COMPLOG_WARNING("Backend config set error response:", errText.toStdString());
                    emit sig_responseConfigRemove(false, errText);
                    return;
                }
                DBRecords::AIBackendInfo pBackendInfo;
                pBackendInfo.readJson(resp->readAll().toStdString());
                emit sig_responseConfigSet(true, {}, pBackendInfo.toPointer());
            });
}

void Client_BackendServiceManager::requestConfigRemove(const DBRecords::AIBackendInfo::id_t &backendId)
{
    auto& requester = getRequester();
    auto req = createRequest(createTarget(Exchange::HTTPv1::QT_BACKEND_CONFIG_REM, backendId));
    auto resp = requester.get(req);
    connect(resp, &QNetworkReply::finished,
            this, [this, resp, backendId](){
                if (resp->error() != QNetworkReply::NoError) {
                    auto errText = resp->readAll();
                    COMPLOG_WARNING("Backend config remove error response:", errText.toStdString());
                    emit sig_responseConfigRemove(false, errText);
                    return;
                }
                emit sig_responseConfigRemove(true, {}, backendId);
            });
}

QString Client_BackendServiceManager::createTarget(const std::string &apiUrl, const DBRecords::AIBackendInfoPtr &pBackend) const
{
    auto res = QString::fromStdString(apiUrl);
    return res.arg(QString::fromStdString(pBackend->getId()));
}

QString Client_BackendServiceManager::createTarget(const std::string &apiUrl, const DBRecords::AIBackendInfo::id_t &backendId) const
{
    auto res = QString::fromStdString(apiUrl);
    return res.arg(QString::fromStdString(backendId));
}
