#include "aimanagerservicewidget.hpp"
#include "ui_aimanagerservicewidget.h"

#include "business/aimanagercontext.hpp"

AIManagerServiceWidget::AIManagerServiceWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AIManagerServiceWidget)
{
    ui->setupUi(this);
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
