#include "aimanagercontext.hpp"

#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Logger/Logger.h>

#include "aibackendservicemanager.hpp"
#include "aibackenddynamicmanager.hpp"

#include "controlwindowsettings.hpp"

AIManagerContext::AIManagerContext(QObject* parent) :
    QObject(parent)
{
    m_pBackendServiceManager = new AIBackendServiceManager(this);

    // DEBUG
    // m_pBackendServiceManager->setDebugEnabled(true);
    // m_pBackendServiceManager->updateBackends();

    m_pBackendDynamicManager = new AIBackendDynamicManager(this);
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
    m_pBackendServiceManager->setAddress(addr);
    m_pBackendDynamicManager->setAddress(addr);

    // Harvest data from a manager
    m_pBackendServiceManager->updateBackends();
}

QString AIManagerContext::getAddress() const
{
    return m_pBackendServiceManager->getAddress();
}

AIBackendServiceManager *AIManagerContext::getBackendServiceManager() const
{
    return m_pBackendServiceManager;
}

AIBackendDynamicManager *AIManagerContext::getBackendDynamicManager() const
{
    return m_pBackendDynamicManager;
}
