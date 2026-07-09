#include "backendmanagementwidget.hpp"
#include "ui_backendmanagementwidget.h"

#include "gui/models/backendtablemodel.hpp"
#include "gui/models/backendtreemodel.hpp"

#include "business/aimanagercontext.hpp"

BackendManagementWidget::BackendManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackendManagementWidget)
{
    ui->setupUi(this);

    m_pManagerContext = new AIManagerContext(this);

    m_pBackendTableModel = new BackendTableModel(this);
    m_pBackendTableModel->setBackendContext(m_pManagerContext);
    m_pBackendTreeModel = new BackendTreeModel(this);
    m_pBackendTreeModel->setSourceModel(m_pBackendTableModel);
    m_pBackendTreeModel->setTreeColumn(BackendTableModel::C_type);

    ui->treeViewBackendTree->setModel(m_pBackendTreeModel);
    ui->treeViewBackendTree->setTreePosition(BackendTableModel::C_type);
    ui->treeViewBackendTree->hideColumn(BackendTableModel::C_id);
    ui->treeViewBackendTree->hideColumn(BackendTableModel::C_address);
}

BackendManagementWidget::~BackendManagementWidget()
{
    delete ui;
}

void BackendManagementWidget::setAIManagerAddress(const QString &addr, uint16_t apiPort)
{
    m_pManagerContext->setAddress(addr + ":" + QString::number(apiPort));
}

QString BackendManagementWidget::getAIManagerAddress() const
{
    return m_pManagerContext->getAddress();
}
