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

void AgentConfigWidget::setConfig(AIObjects::OllamaConfigPtr pConfig)
{
    m_pConfig = pConfig;
}

AIObjects::OllamaConfigPtr AgentConfigWidget::getConfig() const
{
    return m_pConfig;
}
