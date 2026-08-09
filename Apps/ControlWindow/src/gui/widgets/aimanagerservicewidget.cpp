#include "aimanagerservicewidget.hpp"
#include "ui_aimanagerservicewidget.h"

#include <Components/Ecosystem/ApplicationSettings.h>
#include <Components/Logger/Logger.h>

#include <QtConcurrent>

#include "business/aimanagercontext.hpp"
#include "business/controlservicemanager.hpp"

AIManagerServiceWidget::AIManagerServiceWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AIManagerServiceWidget)
{
    ui->setupUi(this);

    connect(ui->pushButtonSaveControlAddress, &QPushButton::clicked,
            this, [this](){
                auto controlAddress = ui->lineEditControlAddress->text() + ":" +
                                      QString::number(ui->spinBoxControlPort->value());
                COMPLOG_INFO("Changing address of control unit to:", controlAddress.toStdString());

                auto& settings = Common::ApplicationSettings::getInstance();
                // TODO: Save connection settings
                m_pManagerContext->setAddress(controlAddress);
            });

    connect(ui->pushButtonShowPassword, &QPushButton::clicked,
            this, [this](){
                auto echoMode = ui->lineEditToken->echoMode();
                if (echoMode == QLineEdit::EchoMode::Normal) {
                    ui->lineEditToken->setEchoMode(QLineEdit::EchoMode::Password);
                    ui->pushButtonShowPassword->setText("Show");
                } else {
                    ui->lineEditToken->setEchoMode(QLineEdit::EchoMode::Normal);
                    ui->pushButtonShowPassword->setText("Hide");
                }
            });

    connect(ui->pushButtonDBShowPassword, &QPushButton::clicked,
            this, [this](){
                auto echoMode = ui->lineEditDBPass->echoMode();
                if (echoMode == QLineEdit::EchoMode::Normal) {
                    ui->lineEditDBPass->setEchoMode(QLineEdit::EchoMode::Password);
                    ui->pushButtonDBShowPassword->setText("Show");
                } else {
                    ui->lineEditDBPass->setEchoMode(QLineEdit::EchoMode::Normal);
                    ui->pushButtonDBShowPassword->setText("Hide");
                }
            });

    connect(ui->pushButtonSaveDBConfig, &QPushButton::clicked,
            this, [this](){
                Exchange::DatabaseConfiguration conf;
                conf.m_dbName       = ui->lineEditDBName->text().toStdString();
                conf.m_dbAddress    = ui->lineEditDBAddress->text().toStdString();
                conf.m_dbPort       = ui->spinBoxDBPort->value();
                conf.m_dbUsername   = ui->lineEditDBUser->text().toStdString();
                conf.m_dbPassword   = ui->lineEditDBPass->text().toStdString();
                m_pManagerContext->getControlServiceManager()->getControlClient()->requrestSetDBParameters(conf);
            });
}

AIManagerServiceWidget::~AIManagerServiceWidget()
{
    delete ui;
}

void AIManagerServiceWidget::setContext(AIManagerContext *pContext)
{
    if (m_pManagerContext) {
        disconnect(m_pManagerContext, nullptr, this, nullptr);
    }
    m_pManagerContext = pContext;

    if (m_pManagerContext) {
        connect(m_pManagerContext, &AIManagerContext::sig_connected,
                this, &AIManagerServiceWidget::fetchConfiguration);
    }
}

AIManagerContext *AIManagerServiceWidget::getManagerContext()
{
    return m_pManagerContext;
}

void AIManagerServiceWidget::fetchConfiguration()
{
    QtConcurrent::run([this](){
        auto pControlClient = m_pManagerContext->getControlServiceManager()->getControlClient();

        auto& tokenF = pControlClient->getToken();
        auto& apiPortF = pControlClient->getPort();
        auto& inputModel = pControlClient->getInputModel();
        auto& dbConfF = pControlClient->getDBConfig();

        auto tokenOpt = tokenF.getValue(1000);
        auto token = QString::fromStdString(tokenOpt.value_or(""));
        auto apiAddr = pControlClient->getServer().split(":").first(); // Expected existance (see connected signal logic)
        auto apiPort = apiPortF.getValue(1000).value_or(1);
        auto dbConf = dbConfF.getValue(1000).value_or(Exchange::DatabaseConfiguration{});

        QMetaObject::invokeMethod(this, [this,
                                         token = std::move(token),
                                         apiAddr = std::move(apiAddr),
                                         apiPort = std::move(apiPort),
                                         dbConf = std::move(dbConf)](){
            ui->lineEditToken->setText(token);

            ui->lineEditControlAddress->setText(apiAddr);
            ui->spinBoxControlPort->setValue(apiPort);

            // TODO: Use comboBoxInputModel to set current input model

            ui->lineEditDBAddress->setText(QString::fromStdString(dbConf.m_dbAddress));
            ui->lineEditDBName->setText(QString::fromStdString(dbConf.m_dbName));
            ui->lineEditDBUser->setText(QString::fromStdString(dbConf.m_dbUsername));
            ui->lineEditDBPass->setText(QString::fromStdString(dbConf.m_dbPassword));
            ui->spinBoxDBPort->setValue(dbConf.m_dbPort);
        });
    });
}
