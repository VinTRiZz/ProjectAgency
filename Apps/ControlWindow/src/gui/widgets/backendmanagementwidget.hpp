#pragma once

#include <QWidget>

#include <Components/CustomQt/Models/TreeGroupingProxyModel.h>

namespace Ui {
class BackendManagementWidget;
}

class BackendTableModel;
class BackendTreeModel;
class AIManagerContext;

class BackendManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackendManagementWidget(QWidget *parent = nullptr);
    ~BackendManagementWidget();

private:
    Ui::BackendManagementWidget *ui;

    AIManagerContext*  m_pManagerContext {nullptr};
    BackendTableModel* m_pBackendTableModel {nullptr};
    BackendTreeModel*  m_pBackendTreeModel {nullptr};
};
