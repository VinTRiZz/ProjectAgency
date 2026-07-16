#include "taskmanagementwidget.hpp"
#include "ui_taskmanagementwidget.h"

TaskManagementWidget::TaskManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TaskManagementWidget)
{
    ui->setupUi(this);
}

TaskManagementWidget::~TaskManagementWidget()
{
    delete ui;
}
