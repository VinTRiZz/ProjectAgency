#include "backendstatuswidget.hpp"
#include "ui_backendstatuswidget.h"

BackendStatusWidget::BackendStatusWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackendStatusWidget)
{
    ui->setupUi(this);
}

BackendStatusWidget::~BackendStatusWidget()
{
    delete ui;
}
