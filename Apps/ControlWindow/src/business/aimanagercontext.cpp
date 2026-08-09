#include "aimanagercontext.hpp"

#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Logger/Logger.h>

#include "aibackendservicemanager.hpp"
#include "aibackenddynamicmanager.hpp"
#include "controlservicemanager.hpp"

#include "controlwindowsettings.hpp"

AIManagerContext::AIManagerContext(QObject* parent) :
    QObject(parent)
{
    m_pControlServiceManager = new ControlServiceManager(this);
    connect(m_pControlServiceManager, &ControlServiceManager::sig_errorOccurs,
            this, &AIManagerContext::sig_errorOccurs);
    connect(m_pControlServiceManager, &ControlServiceManager::sig_connected,
            this, &AIManagerContext::updateConnectedCounter);

    m_pBackendServiceManager = new AIBackendServiceManager(this);
    connect(m_pBackendServiceManager, &AIBackendServiceManager::sig_errorOccurs,
            this, &AIManagerContext::sig_errorOccurs);
    connect(m_pBackendServiceManager, &AIBackendServiceManager::sig_connected,
            this, &AIManagerContext::updateConnectedCounter);

    // DEBUG
    // m_pBackendServiceManager->setDebugEnabled(true);
    // m_pBackendServiceManager->updateBackends();

    m_pBackendDynamicManager = new AIBackendDynamicManager(this);
    connect(m_pBackendDynamicManager, &AIBackendDynamicManager::sig_errorOccurs,
            this, &AIManagerContext::sig_errorOccurs);
    connect(m_pBackendDynamicManager, &AIBackendDynamicManager::sig_connected,
            this, &AIManagerContext::updateConnectedCounter);
}

void AIManagerContext::init()
{
    auto& settingsInstance = Common::ApplicationSettings::getInstance();

    auto lv_initSetting = [&settingsInstance](const auto& sect, const auto& name, const auto initV) {
        if (!settingsInstance.hasSetting(sect, name)) {
            settingsInstance.addSetting(sect, name);
        }
        auto pSetting = settingsInstance.getSetting(sect, name);
        if (!pSetting->isSet()) {
            pSetting->setValue(initV);
        }
    };

    lv_initSetting(ControlWindowSettings::SECTION_AI_BACKEND,
                   ControlWindowSettings::AI_BACKEND_ADDRESS,
                   "127.0.0.1");
    auto backendAddrSett = settingsInstance.getSetting(
        ControlWindowSettings::SECTION_AI_BACKEND,
        ControlWindowSettings::AI_BACKEND_ADDRESS);

    lv_initSetting(ControlWindowSettings::SECTION_AI_BACKEND,
                   ControlWindowSettings::AI_BACKEND_PORT,
                   9001);
    auto backendPortSett = settingsInstance.getSetting(
        ControlWindowSettings::SECTION_AI_BACKEND,
        ControlWindowSettings::AI_BACKEND_PORT);

    if (backendAddrSett->isSet() &&
        backendPortSett->isSet()) {
        setAddress(QString::fromStdString(
            backendAddrSett->getValueString() + ":" +
            backendPortSett->getValueString()));
    }
    settingsInstance.saveSettings();
}

void AIManagerContext::setAddress(const QString &addr)
{
    COMPLOG_INFO("Context address changed to:", addr.toStdString());
    resetConnectedCounter();

    auto addSplit = addr.split(":", Qt::SplitBehaviorFlags::SkipEmptyParts);
    if (addSplit.size() != 2) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::InterfaceInvalidAddress, "Invalid address of control unit"));
        return;
    }
    if (!QtCustom::Web::HTTPClientBase::isServerListening(addSplit.front(), addSplit.back().toInt())) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::InterfaceConnectionError, "Failed to connect to control unit"));
        return;
    }

    m_pControlServiceManager->setAddress(addr);
    m_pBackendServiceManager->setAddress(addr);
    m_pBackendDynamicManager->setAddress(addr);

    // Harvest data from a manager
    m_pBackendServiceManager->updateBackends();
}

QString AIManagerContext::getAddress() const
{
    return m_pBackendServiceManager->getAddress();
}

ControlServiceManager *AIManagerContext::getControlServiceManager() const
{
    return m_pControlServiceManager;
}

AIBackendServiceManager *AIManagerContext::getBackendServiceManager() const
{
    return m_pBackendServiceManager;
}

AIBackendDynamicManager *AIManagerContext::getBackendDynamicManager() const
{
    return m_pBackendDynamicManager;
}

void AIManagerContext::resetConnectedCounter()
{
    m_connectedInstances.store(0);
}

void AIManagerContext::updateConnectedCounter()
{
    m_connectedInstances.fetch_add(1);
    if (m_connectedInstances.load() == 3) {
        emit sig_connected();
    }
}
