#pragma once

#include <QMenu>
#include <QAbstractItemModel>
#include <QAbstractItemView>

#include <ProjectAgency/DB/AIBackendInfo.h>

class BackendTreeModel;

/**
 * @brief The BackendContextMenu class Context menu to work with backends
 */
class BackendContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit BackendContextMenu(QWidget* parent = nullptr);
    ~BackendContextMenu();

    // Sets index menu called on. Must be called before executing, configures actions
    void setTargetIndex(const QModelIndex& idx);
    QModelIndex getTargetIndex() const;

    void setModel(BackendTreeModel* pModel);
    void setView(QAbstractItemView* pView);

signals:
    void sig_errorOccurs(const Exchange::Error& err) const;
    void sig_addBackendRequested(const DBRecords::AIBackendInfoPtr& pBackend) const;
    void sig_editBackendRequested(const DBRecords::AIBackendInfoPtr& pBackend) const;
    void sig_removeBackendRequested(const DBRecords::AIBackendInfoPtr& pBackend) const;

private slots:
    void slot_addBackend() const;
    void slot_editBackend() const;
    void slot_removeBackend() const;

private:
    BackendTreeModel* m_pModel {nullptr};
    QAbstractItemView*  m_pView {nullptr};
    QModelIndex m_targetIndex {}; // Index context menu called on

    // To enable / disable
    QAction* m_pEditAction {nullptr};
    QAction* m_pRemoveAction {nullptr};
};
