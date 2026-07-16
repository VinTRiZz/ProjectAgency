#include "airolemanagementwidget.hpp"
#include "ui_airolemanagementwidget.h"

AIRoleManagementWidget::AIRoleManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AIRoleManagementWidget)
{
    ui->setupUi(this);
}

AIRoleManagementWidget::~AIRoleManagementWidget()
{
    delete ui;
}
