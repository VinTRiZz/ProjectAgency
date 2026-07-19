#include "backendmanagementwidget.hpp"
#include "ui_backendmanagementwidget.h"

#include <QMessageBox>

#include "gui/models/backendtablemodel.hpp"
#include "gui/models/backendtreemodel.hpp"

#include "gui/context/backendcontextmenu.hpp"
#include "gui/dialogs/backendconfigurationdialog.hpp"

#include "business/aimanagercontext.hpp"
#include "client/client_backendservicemanager.hpp"


BackendManagementWidget::BackendManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackendManagementWidget)
{
    ui->setupUi(this);

    setupBackendTree();
}

BackendManagementWidget::~BackendManagementWidget()
{
    delete ui;
}

void BackendManagementWidget::setContext(AIManagerContext *pContext)
{
    if (m_pManagerContext) {
        disconnect(m_pManagerContext->getBackendServiceManager(), nullptr, this, nullptr);
    }
    m_pManagerContext = pContext;
    if (m_pManagerContext) {
        connect(m_pManagerContext->getBackendServiceManager(), &AIBackendServiceManager::sig_errorOccurs,
                this, &BackendManagementWidget::slot_processError);
    }

    m_pBackendTableModel->setBackendContext(m_pManagerContext);
}

AIManagerContext *BackendManagementWidget::getManagerContext()
{
    return m_pManagerContext;
}

void BackendManagementWidget::slot_processError(const Exchange::Error &err)
{
    if (!m_pErrorMessageBox) {
        m_pErrorMessageBox = new QMessageBox(this);
        m_pErrorMessageBox->setWindowTitle("Operation failed");
        m_pErrorMessageBox->setIcon(QMessageBox::Critical);
    }
    m_pErrorMessageBox->setText(err.getErrorText().c_str());
    m_pErrorMessageBox->setDetailedText(err.getDetailText().c_str());
    m_pErrorMessageBox->exec();
}

void BackendManagementWidget::setupBackendTree()
{
    m_pBackendTableModel = new BackendTableModel(this);
    m_pBackendTreeModel = new BackendTreeModel(this);
    m_pBackendTreeModel->setSourceModel(m_pBackendTableModel);
    m_pBackendTreeModel->setTreeColumn(BackendTableModel::C_name);

    ui->treeViewBackendTree->setModel(m_pBackendTreeModel);
    ui->treeViewBackendTree->setTreePosition(BackendTableModel::C_name);
    ui->treeViewBackendTree->hideColumn(BackendTableModel::C_type); // Will be as a decoration
    ui->treeViewBackendTree->hideColumn(BackendTableModel::C_id);
    ui->treeViewBackendTree->hideColumn(BackendTableModel::C_address);
    ui->treeViewBackendTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    setupBackendContextMenu();
}

void BackendManagementWidget::setupBackendContextMenu()
{
    m_pBackendContextMenu = new BackendContextMenu(this);
    m_pBackendContextMenu->setModel(m_pBackendTreeModel);
    m_pBackendContextMenu->setView(ui->treeViewBackendTree);
    connect(m_pBackendContextMenu, &BackendContextMenu::sig_errorOccurs,
            this, &BackendManagementWidget::slot_processError);
    connect(m_pBackendContextMenu, &BackendContextMenu::sig_addBackendRequested,
            this, [this](auto pBackend){
                if (!m_pManagerContext) {
                    slot_processError(Exchange::Error(Exchange::ErrorCode::GuiServerProcessingFail, "Context not set"));
                    return;
                }
                BackendConfigurationDialog confDialog {};
                confDialog.setBackend(*pBackend);
                auto execRes = confDialog.exec();
                if (QDialog::Accepted == execRes) {
                    *pBackend = confDialog.getBackend();
                    m_pManagerContext->getBackendServiceManager()->getClient()->requestConfigAdd(pBackend);
                }
            });
    connect(m_pBackendContextMenu, &BackendContextMenu::sig_editBackendRequested,
            this, [this](auto pBackend){
                if (!m_pManagerContext) {
                    slot_processError(Exchange::Error(Exchange::ErrorCode::GuiServerProcessingFail, "Context not set"));
                    return;
                }
                BackendConfigurationDialog confDialog {};
                confDialog.setBackend(*pBackend);
                auto execRes = confDialog.exec();
                if (QDialog::Accepted == execRes) {
                    *pBackend = confDialog.getBackend();
                    m_pManagerContext->getBackendServiceManager()->getClient()->requestConfigSet(pBackend);
                }
            });
    connect(m_pBackendContextMenu, &BackendContextMenu::sig_removeBackendRequested,
            this, [this](auto pBackend){
        if (!m_pManagerContext) {
            slot_processError(Exchange::Error(Exchange::ErrorCode::GuiServerProcessingFail, "Context not set"));
            return;
        }
        m_pManagerContext->getBackendServiceManager()->getClient()->requestConfigRemove(pBackend->getId());
    });
    ui->treeViewBackendTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewBackendTree, &QWidget::customContextMenuRequested,
            m_pBackendContextMenu, [this](const QPoint& menuPos){
                m_pBackendContextMenu->setTargetIndex(ui->treeViewBackendTree->currentIndex()); // Current is selected always
                m_pBackendContextMenu->exec(ui->treeViewBackendTree->viewport()->mapToGlobal(menuPos));
            });
}
