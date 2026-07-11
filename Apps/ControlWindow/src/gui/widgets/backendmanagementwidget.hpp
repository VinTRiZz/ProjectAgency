#pragma once

#include <QWidget>

#include <Components/CustomQt/Models/TreeGroupingProxyModel.h>

#include <ProjectAgency/Exchange/Error.h>

namespace Ui {
class BackendManagementWidget;
}

class BackendTableModel;
class BackendTreeModel;
class AIManagerContext;
class BackendContextMenu;
class QMessageBox;

class BackendManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackendManagementWidget(QWidget *parent = nullptr);
    ~BackendManagementWidget();

    void setAIManagerAddress(const QString& addr, uint16_t apiPort);
    QString getAIManagerAddress() const;

private slots:
    void slot_processError(const Exchange::Error& err);

private:
    Ui::BackendManagementWidget *ui;

    QMessageBox* m_pErrorMessageBox {nullptr};

    AIManagerContext*  m_pManagerContext {nullptr};
    BackendTableModel* m_pBackendTableModel {nullptr};
    BackendTreeModel*  m_pBackendTreeModel {nullptr};
    BackendContextMenu* m_pBackendContextMenu {nullptr};
};
