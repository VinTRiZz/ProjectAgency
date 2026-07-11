#include "backendconfigurationdialog.hpp"
#include "ui_backendconfigurationdialog.h"

BackendConfigurationDialog::BackendConfigurationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BackendConfigurationDialog)
{
    ui->setupUi(this);

    connect(ui->pushButtonSave, &QPushButton::clicked,
            this, &QDialog::accept);
    connect(ui->pushButtonRevert, &QPushButton::clicked,
            this, &BackendConfigurationDialog::resetInfo);
    connect(ui->pushButtonExit, &QPushButton::clicked,
            this, &QDialog::reject);
}

BackendConfigurationDialog::~BackendConfigurationDialog()
{
    delete ui;
}

void BackendConfigurationDialog::setBackend(const DBRecords::AIBackendInfo &backInfo)
{
    m_currentBackendInfo = backInfo;
    resetInfo();
}

DBRecords::AIBackendInfo BackendConfigurationDialog::getBackend() const
{
    m_currentBackendInfo.setIp(ui->lineEditAddress->text().toStdString());
    m_currentBackendInfo.setPort(ui->spinBoxPort->value());
    m_currentBackendInfo.setDisplayName(ui->lineEditName->text().toStdString());
    m_currentBackendInfo.setType(DBRecords::AIBackendDeviceType(ui->comboBoxType->currentIndex()));
    return m_currentBackendInfo;
}

void BackendConfigurationDialog::resetInfo()
{
    ui->lineEditAddress->setText(m_currentBackendInfo.getIp().c_str());
    ui->spinBoxPort->setValue(m_currentBackendInfo.getPort());
    ui->lineEditName->setText(m_currentBackendInfo.getDisplayName().c_str());
    ui->comboBoxType->setCurrentIndex(m_currentBackendInfo.getType());
}
