#include "aimanagerservicewidget.hpp"
#include "ui_aimanagerservicewidget.h"

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
