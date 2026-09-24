#pragma once

#include <QAbstractTableModel>

#include <Components/ExtraClasses/Containers/Handler.h>

#include "business/aibackendservicemanager.hpp"

class AIManagerContext;

/**
 * @brief The BackendTableModel class Backend instances model to view AIManager's contains
 */
class BackendTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BackendTableModel(QObject *parent = nullptr);

    enum Columns : int {
        C_id = 0,
        C_address,
        C_type,
        C_name,
        C_status, // Work / idle, etc.

        C_SYS_columnCount // To extend easier (used in columnCount)
    };

    enum Roles : int {
        R_isOnline = Qt::UserRole + 1, // affects decoration role
        R_id,
        R_address,
        R_type,
        R_name,
        R_status, // Work / idle, etc.
        R_backendPtr,
    };

    // QAbstractTableModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setBackendContext(AIManagerContext* pContext);
    DBRecords::AIBackendInfoPtr getBackend(int row) const;
    DBRecords::AIBackendInfoPtr getBackend(const QModelIndex& idx) const;

private:
    AIManagerContext*   m_pBackendContext {nullptr};
    BackendArray        m_backends;
};

