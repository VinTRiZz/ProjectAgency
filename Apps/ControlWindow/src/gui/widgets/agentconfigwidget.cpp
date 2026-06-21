#include "agentconfigwidget.hpp"
#include "ui_agentconfigwidget.h"

AgentConfigWidget::AgentConfigWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AgentConfigWidget)
{
    ui->setupUi(this);
}

AgentConfigWidget::~AgentConfigWidget()
{
    delete ui;
}

void AgentConfigWidget::setConfig(DataObjects::OllamaConfigPtr pConfig)
{
    m_pConfig = pConfig;
}

DataObjects::OllamaConfigPtr AgentConfigWidget::getConfig() const
{
    return m_pConfig;
}
