#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "business/aimanagercontext.hpp"

#include <ProjectAgency/Exchange/Error.h>

#include <Components/Logger/Logger.h>

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pManagerContext = new AIManagerContext(this); 
    connect(m_pManagerContext, &AIManagerContext::sig_errorOccurs,
            this, &MainWindow::slot_processError);

    ui->tabWidget->setCurrentIndex(0);

    ui->aiManagerServiceWidget->setContext(m_pManagerContext);
    ui->backendManagementWidget->setContext(m_pManagerContext);

    m_pManagerContext->init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_processError(const Exchange::Error &err)
{
    COMPLOG_ERROR_SYNC("USER MESSAGE:", err.what());
    if (!m_pErrorMessageBox) {
        m_pErrorMessageBox = new QMessageBox(this);
        m_pErrorMessageBox->setWindowTitle("Operation failed");
        m_pErrorMessageBox->setIcon(QMessageBox::Critical);
    }
    m_pErrorMessageBox->setText(err.getErrorText().c_str());
    m_pErrorMessageBox->setDetailedText(err.getDetailText().c_str());
    m_pErrorMessageBox->exec();
}
