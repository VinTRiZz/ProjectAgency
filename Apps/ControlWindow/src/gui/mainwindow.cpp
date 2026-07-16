#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    // TODO: Add a form to setup backend
    ui->backendManagementWidget->setAIManagerAddress("127.0.0.1", 9001);
}

MainWindow::~MainWindow()
{
    delete ui;
}
