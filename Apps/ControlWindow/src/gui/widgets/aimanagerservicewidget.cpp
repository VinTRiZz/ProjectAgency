#include "aimanagerservicewidget.hpp"
#include "ui_aimanagerservicewidget.h"

#include <Components/Ecosystem/ApplicationSettings.h>

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

    connect(ui->pushButtonSaveDBConfig, &QPushButton::clicked,
            this, [this](){
        m_pManagerContext->getControlServiceManager()->setDatabaseConfiguration({}); // TODO: Set
    });
}

AIManagerServiceWidget::~AIManagerServiceWidget()
{
    delete ui;
}

void AIManagerServiceWidget::setContext(AIManagerContext *pContext)
{
    m_pManagerContext = pContext;
}

AIManagerContext *AIManagerServiceWidget::getManagerContext()
{
    return m_pManagerContext;
}
