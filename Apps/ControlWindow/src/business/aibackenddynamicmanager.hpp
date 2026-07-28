#pragma once

#include <QObject>

#include <ProjectAgency/DB/AIBackendInfo.h>

class AIBackendServiceManager;

/**
 * @brief The AIBackendDynamicManager class Object to get AIBackend online status, current tasks, system status, etc.
 */
class AIBackendDynamicManager : public QObject
{
    Q_OBJECT
public:
    explicit AIBackendDynamicManager(QObject* parent = nullptr);

    void setAddress(const QString& addr);
    QString getAddress() const;

    void setServiceManager(AIBackendServiceManager* pManager);

    bool isBackendOnline(const DBRecords::AIBackendInfo::id_nullable_t& id) const;

signals:
    void sig_connected() const;
    void sig_errorOccurs(const Exchange::Error& err) const;

private:
    AIBackendServiceManager* m_pServiceManager {nullptr};
};
