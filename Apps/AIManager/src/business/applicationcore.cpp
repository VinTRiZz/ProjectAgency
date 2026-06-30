#include "applicationcore.hpp"

#include <Components/Logger/Logger.h>

#include "aimanager.hpp"
#include "airolemanager.hpp"

#include "httpcontrollers/aiservicecontroller.hpp"
#include "httpcontrollers/aistatuscontroller.hpp"
#include "httpcontrollers/userrequestcontroller.hpp"

struct ApplicationCore::Impl
{
    Database::RecordManagerPtr m_pRecordManager;

    AIManager m_aiManager;
    AIRoleManager m_aiRoleManager;
};

ApplicationCore::ApplicationCore() :
    d {new Impl}
{

}

ApplicationCore::~ApplicationCore()
{

}

void ApplicationCore::setToken(const std::string &tokenString)
{

}

bool ApplicationCore::init()
{
    // TODO: Move into config
    COMPLOG_INFO_SYNC("Configuring DB connection...");
    d->m_pRecordManager = std::make_shared<Database::RecordManager>();
    auto& con = d->m_pRecordManager->getConnection();
    con.setAppName("AIManager");
    con.setName("main");
    con.setUser("server", "serv_auth_password");
    con.setServer("127.0.0.1", 10001);
    con.setDatabase("pag_main");
    con.init();

    // Setup roles
    COMPLOG_INFO_SYNC("Reading roles from DB...");
    d->m_aiRoleManager.setRecordManager(d->m_pRecordManager);
    d->m_aiRoleManager.readDatabase();

    // Setup AI roles
    COMPLOG_INFO_SYNC("Reading AIBackend configurations...");
    d->m_aiManager.setRecordManager(d->m_pRecordManager);
    d->m_aiManager.init();

    COMPLOG_OK("AIManager init complete");
    return true;
}

void ApplicationCore::start(uint16_t apiPort)
{
    d->m_aiManager.start();

    // Controller setup
    drogon::app().registerController(std::make_shared<AIServiceController>(d->m_aiManager));
    drogon::app().registerController(std::make_shared<AIStatusController>(d->m_aiManager));
    drogon::app().registerController(std::make_shared<UserRequestController>(d->m_aiManager));

    // Server info
    drogon::app().setServerHeaderField("AIManager");

    // 3 threads, one for pending operations, second for periodic requests, third is extra
    drogon::app().setThreadNum(3);

    drogon::app().addListener("0.0.0.0", apiPort);
    drogon::app().run();
}

void ApplicationCore::stop()
{
    if (!drogon::app().isRunning()) {
        return;
    }
    d->m_aiManager.stop();
    drogon::app().quit();
}
