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
        auto pClient = m_pManagerContext->getControlServiceManager()->getControlClient();
        connect(pClient, &Client_ControlServiceManager::sig_configChanged,
                this, &AIManagerServiceWidget::updateConfiguration);
    }
}

AIManagerContext *AIManagerServiceWidget::getManagerContext()
{
    return m_pManagerContext;
}

void AIManagerServiceWidget::updateConfiguration()
{
    auto pClient = m_pManagerContext->getControlServiceManager()->getControlClient();

    ui->lineEditToken->setText(QString::fromStdString(pClient->getToken()));

    ui->lineEditControlAddress->setText(pClient->getServer());
    ui->spinBoxControlPort->setValue(pClient->getPort());

    // TODO: Use comboBoxInputModel to set current input model

    auto conf = pClient->getDBConfig();
    ui->lineEditDBAddress->setText(QString::fromStdString(conf.m_dbAddress));
    ui->lineEditDBName->setText(QString::fromStdString(conf.m_dbName));
    ui->lineEditDBUser->setText(QString::fromStdString(conf.m_dbUsername));
    ui->lineEditDBPass->setText(QString::fromStdString(conf.m_dbPassword));
    ui->spinBoxDBPort->setValue(conf.m_dbPort);
}
