#include "backendcontextmenu.hpp"

#include <QAction>

#include <Components/Logger/Logger.h>

#include "gui/models/backendtablemodel.hpp"
#include "gui/models/backendtreemodel.hpp"

BackendContextMenu::BackendContextMenu(QWidget* parent) :
    QMenu(parent)
{
    addAction("Add backend", this, &BackendContextMenu::slot_addBackend);
    m_pEditAction = addAction("Edit configuration", this, &BackendContextMenu::slot_editBackend);
    m_pRemoveAction = addAction("Remove backend", this, &BackendContextMenu::slot_removeBackend);
}

BackendContextMenu::~BackendContextMenu()
{

}

void BackendContextMenu::setTargetIndex(const QModelIndex &idx)
{
    if (!m_pModel) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::GuiModelInvalidModel, "No model set to process index"));
        return;
    }
    m_targetIndex = idx;

    auto pBackend = m_targetIndex.data(BackendTableModel::R_backendPtr).value<DBRecords::AIBackendInfoPtr>();
    auto isBackendSelected = (nullptr != pBackend);
    m_pEditAction->setEnabled(isBackendSelected);
    m_pRemoveAction->setEnabled(isBackendSelected);
}

QModelIndex BackendContextMenu::getTargetIndex() const
{
    return m_targetIndex;
}

void BackendContextMenu::setModel(BackendTreeModel *pModel)
{
    m_pModel = pModel;
}

void BackendContextMenu::setView(QAbstractItemView *pView)
{
    m_pView = pView;
}

void BackendContextMenu::slot_addBackend() const
{
    if (!m_pModel) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::GuiModelInvalidModel, "No model set to call signal"));
        return;
    }

    if (!m_targetIndex.isValid()) {
        emit sig_addBackendRequested(DBRecords::AIBackendInfo::create());
        return;
    }

    uint8_t indexLevel {};
    auto curIndex = m_targetIndex;
    while (curIndex.parent().isValid()) {
        ++indexLevel;
        curIndex = curIndex.parent();
    }
    curIndex = m_targetIndex;
    while (curIndex.isValid() && m_pModel->rowCount(curIndex)) {
        curIndex = m_pModel->index(0, 0, curIndex);
    }

    // Preconfigure backend according to existing data
    DBRecords::AIBackendInfo backendPreconfig {};
    auto maxLevel = m_pModel->getMaxGroupLevel();
    for (uint8_t curLevel = 0; curLevel <= indexLevel && curLevel <= maxLevel; ++curLevel) {
        auto opt_groupRule = m_pModel->getGroupingRule(maxLevel - curLevel);
        if (!opt_groupRule.has_value()) {
            continue;
        }
        auto groupRule = opt_groupRule.value();
        switch (groupRule)
        {
        case BackendTreeModel::GR_type:
            backendPreconfig.setType(DBRecords::AIBackendDeviceType(curIndex.data(BackendTableModel::R_type).toInt()));
            break;
        case BackendTreeModel::GR_name:
            backendPreconfig.setDisplayName(curIndex.data(BackendTableModel::R_name).toString().toStdString());
            break;
        case BackendTreeModel::GR_ip:
            backendPreconfig.setIp(curIndex.data(BackendTableModel::R_address).toString().toStdString());
            break;
        default: continue;
        }
    }
    emit sig_addBackendRequested(backendPreconfig.toPointer());
}

void BackendContextMenu::slot_editBackend() const
{
    auto pBackend = m_targetIndex.data(BackendTableModel::R_backendPtr).value<DBRecords::AIBackendInfoPtr>();
    if (!pBackend) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::GuiModelInvalidIndex, "No backend selected"));
        return;
    }
    emit sig_editBackendRequested(pBackend);
}

void BackendContextMenu::slot_removeBackend() const
{
    if (!m_pModel) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::GuiModelInvalidModel, "No model set"));
        return;
    }
    auto pBackend = m_targetIndex.data(BackendTableModel::R_backendPtr).value<DBRecords::AIBackendInfoPtr>();
    if (!pBackend) {
        emit sig_errorOccurs(Exchange::Error(Exchange::ErrorCode::GuiModelInvalidIndex, "No backend selected"));
        return;
    }
    COMPLOG_INFO("BackendContextMenu: Removing backend", pBackend->getId(), "(", pBackend->getDisplayName(), ")");
    m_pModel->removeRow(m_targetIndex.row(), m_targetIndex.parent());
}
