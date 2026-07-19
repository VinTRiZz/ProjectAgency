#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "business/aimanagercontext.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pManagerContext = new AIManagerContext(this);
    m_pManagerContext->init();

    ui->tabWidget->setCurrentIndex(0);

    ui->backendManagementWidget->setContext(m_pManagerContext);
}

MainWindow::~MainWindow()
{
    delete ui;
}
