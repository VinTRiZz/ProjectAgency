#pragma once

#include <QObject>

class AIBackendServiceManager;
class AIBackendDynamicManager;
class ControlServiceManager;

namespace Exchange {
class Error;
}

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

    ControlServiceManager*   getControlServiceManager() const;
    AIBackendServiceManager* getBackendServiceManager() const;
    AIBackendDynamicManager* getBackendDynamicManager() const;

signals:
    void sig_errorOccurs(const Exchange::Error& err);

private:
    ControlServiceManager*   m_pControlServiceManager {nullptr};
    AIBackendServiceManager* m_pBackendServiceManager {nullptr};
    AIBackendDynamicManager* m_pBackendDynamicManager {nullptr};
};
