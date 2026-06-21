#include "backendmanagementwidget.hpp"
#include "ui_backendmanagementwidget.h"

BackendManagementWidget::BackendManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackendManagementWidget)
{
    ui->setupUi(this);
}

BackendManagementWidget::~BackendManagementWidget()
{
    delete ui;
}
