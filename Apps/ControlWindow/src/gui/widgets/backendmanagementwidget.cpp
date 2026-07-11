#include "backendmanagementwidget.hpp"
#include "ui_backendmanagementwidget.h"

#include <QMessageBox>

#include "gui/models/backendtablemodel.hpp"
#include "gui/models/backendtreemodel.hpp"

#include "gui/context/backendcontextmenu.hpp"

#include "business/aimanagercontext.hpp"


BackendManagementWidget::BackendManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackendManagementWidget)
{
    ui->setupUi(this);

    m_pManagerContext = new AIManagerContext(this);
    connect(m_pManagerContext->getBackendServiceManager(), &AIBackendServiceManager::sig_errorOccurs,
            this, &BackendManagementWidget::slot_processError);

    m_pBackendTableModel = new BackendTableModel(this);
    m_pBackendTableModel->setBackendContext(m_pManagerContext);
    m_pBackendTreeModel = new BackendTreeModel(this);
    m_pBackendTreeModel->setSourceModel(m_pBackendTableModel);
    m_pBackendTreeModel->setTreeColumn(BackendTableModel::C_type);

    ui->treeViewBackendTree->setModel(m_pBackendTreeModel);
    ui->treeViewBackendTree->setTreePosition(BackendTableModel::C_type);
    ui->treeViewBackendTree->hideColumn(BackendTableModel::C_id);
    ui->treeViewBackendTree->hideColumn(BackendTableModel::C_address);
    ui->treeViewBackendTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_pBackendContextMenu = new BackendContextMenu(this);
    m_pBackendContextMenu->setModel(m_pBackendTreeModel);
    m_pBackendContextMenu->setView(ui->treeViewBackendTree);
    connect(m_pBackendContextMenu, &BackendContextMenu::sig_errorOccurs,
            this, &BackendManagementWidget::slot_processError);
    ui->treeViewBackendTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewBackendTree, &QWidget::customContextMenuRequested,
            m_pBackendContextMenu, [this](const QPoint& menuPos){
        m_pBackendContextMenu->setTargetIndex(ui->treeViewBackendTree->currentIndex()); // Current is selected always
        m_pBackendContextMenu->exec(ui->treeViewBackendTree->viewport()->mapToGlobal(menuPos));
    });
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

void BackendManagementWidget::slot_processError(const Exchange::Error &err)
{
    if (!m_pErrorMessageBox) {
        m_pErrorMessageBox = new QMessageBox(this);
        m_pErrorMessageBox->setWindowTitle("Operation failed");
        m_pErrorMessageBox->setIcon(QMessageBox::Critical);
    }
    m_pErrorMessageBox->setText(err.getErrorText().c_str());
    m_pErrorMessageBox->setDetailedText(err.getErrorDetailText().c_str());
    m_pErrorMessageBox->exec();
}
