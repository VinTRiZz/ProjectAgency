#pragma once

#include <QDialog>

#include <ProjectAgency/DB/AIBackendInfo.h>

namespace Ui {
class BackendConfigurationDialog;
}

/**
 * @brief The BackendConfigurationDialog class Dialog to edit info about backend
 */
class BackendConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackendConfigurationDialog(QWidget *parent = nullptr);
    ~BackendConfigurationDialog();

    void setBackend(const DBRecords::AIBackendInfo& backInfo);
    DBRecords::AIBackendInfo getBackend() const;

private slots:
    void resetInfo();

private:
    Ui::BackendConfigurationDialog *ui;

    // Mutable only to set values when requested (in getBacend method)
    mutable DBRecords::AIBackendInfo m_currentBackendInfo {};
};
