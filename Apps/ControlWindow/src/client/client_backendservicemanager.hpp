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
    void requestConfigGet(const DBRecords::AIBackendInfo::id_nullable_t& backendId);
    void requestConfigSet(const DBRecords::AIBackendInfoPtr& pBackendInfo);
    void requestConfigRemove(const DBRecords::AIBackendInfo::id_nullable_t& backendId);

signals:
    void sig_responseIdList(const std::vector<DBRecords::AIBackendInfo::id_nullable_t>& ids = {});
    void sig_responseConfigAdd(const DBRecords::AIBackendInfoPtr& pBackendInfo = {});
    void sig_responseConfigGet(const DBRecords::AIBackendInfoPtr& pBackendInfo = {});
    void sig_responseConfigSet(const DBRecords::AIBackendInfoPtr& pBackendInfo = {});
    void sig_responseConfigRemove(const DBRecords::AIBackendInfo::id_nullable_t& id = {});

private:
    QString createTarget(const std::string& apiUrl, const DBRecords::AIBackendInfoPtr& pBackend) const;
    QString createTarget(const std::string& apiUrl, const DBRecords::AIBackendInfo::id_nullable_t& backendId) const;

    void emitError(const QString& errText) const;
};
