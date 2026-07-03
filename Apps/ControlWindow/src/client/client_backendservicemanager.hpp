#pragma once

#include "client_base/httpclientbase.hpp"

#include <ProjectAgency/DB/AIBackendInfo.h>

class Client_BackendServiceManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit Client_BackendServiceManager(QObject *parent = nullptr);

public slots:
    void requestIdList();
    void requestConfigAdd(const DBRecords::AIBackendInfoPtr& pBackendInfo);
    void requestConfigGet(const DBRecords::AIBackendInfo::id_t& backendId);
    void requestConfigSet(const DBRecords::AIBackendInfoPtr& pBackendInfo);
    void requestConfigRemove(const DBRecords::AIBackendInfo::id_t& backendId);

signals:
    void sig_responseIdList(bool isSucceed, const QString& errorMsg, const std::vector<DBRecords::AIBackendInfo::id_t>& ids = {});
    void sig_responseConfigAdd(bool isSucceed,  const QString& errorMsg, const DBRecords::AIBackendInfoPtr& pBackendInfo = {});
    void sig_responseConfigGet(bool isSucceed,  const QString& errorMsg, const DBRecords::AIBackendInfoPtr& pBackendInfo = {});
    void sig_responseConfigSet(bool isSucceed,  const QString& errorMsg, const DBRecords::AIBackendInfoPtr& pBackendInfo = {});
    void sig_responseConfigRemove(bool isSucceed, const QString& errorMsg, const DBRecords::AIBackendInfo::id_t& id = {});

private:
    QString createTarget(const std::string& apiUrl, const DBRecords::AIBackendInfoPtr& pBackend) const;
    QString createTarget(const std::string& apiUrl, const DBRecords::AIBackendInfo::id_t& backendId) const;
};
