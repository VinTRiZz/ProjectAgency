#pragma once

#include <QObject>

#include <ProjectAgency/DB/AIBackendInfo.h>

/**
 * @brief The AIBackendDynamicManager class Object to get AIBackend online status, current tasks, system status, etc.
 */
class AIBackendDynamicManager : public QObject
{
    Q_OBJECT
public:
    explicit AIBackendDynamicManager(QObject* parent = nullptr);

    bool isBackendOnline(const DBRecords::AIBackendInfo::id_t& id) const;

signals:
    void sig_errorOccurs(const QString& errorText);
};
