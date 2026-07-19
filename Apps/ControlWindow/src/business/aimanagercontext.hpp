#pragma once

#include <QObject>

class AIBackendServiceManager;
class AIBackendDynamicManager;

/**
 * @brief The AIManagerContext class Context of an AIManager remote instance
 */
class AIManagerContext : public QObject
{
    Q_OBJECT
public:
    explicit AIManagerContext(QObject* parent = nullptr);

    void init();

    /**
     * @brief setAddress Address is IP:port
     * @param addr
     */
    void setAddress(const QString& addr);
    QString getAddress() const;

    AIBackendServiceManager* getBackendServiceManager() const;
    AIBackendDynamicManager* getBackendDynamicManager() const;

private:
    AIBackendServiceManager* m_pBackendServiceManager {nullptr};
    AIBackendDynamicManager* m_pBackendDynamicManager {nullptr};
};
