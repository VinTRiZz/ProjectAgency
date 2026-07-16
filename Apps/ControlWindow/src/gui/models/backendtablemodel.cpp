#include "backendtablemodel.hpp"

#include <QColor>

#include <ProjectAgency/DB/AIBackendInfo.h>

#include "business/aimanagercontext.hpp"
#include "business/aibackendservicemanager.hpp"
#include "business/aibackenddynamicmanager.hpp"

BackendTableModel::BackendTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant BackendTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        switch (section)
        {
        case C_id:      return "ID";
        case C_address: return "Address";
        case C_type:    return "Device type";
        case C_name:    return "Name";
        case C_status:  return "Status";
        }
        return {};
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

int BackendTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_backends.size();
}

int BackendTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Columns::C_SYS_columnCount;
}

QVariant BackendTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto pBackend = getBackend(index);
        if (!pBackend) {
            return {"[ERR]"};
        }
        switch (index.column())
        {
        case C_id:      return data(index, R_id);
        case C_address: return data(index, R_address);
        case C_type:    return data(index, R_type);
        case C_name:    return data(index, R_name);
        case C_status:  return data(index, R_status);
        }
        return {};
    }

    if (role == Qt::ToolTipRole) {
        switch (index.column())
        {
        case C_id:      return "ID of a backend";
        case C_address: return "IP Address with port of a backend";
        case C_type:    return "Backend device type (for example, PC or Android)";
        case C_name:    return "Display name of a backend";
        case C_status:  return "Status of backend work";
        }
        return {};
    }

    if (role == Qt::DecorationRole) {
        if (index.column() != 0) {
            return {};
        }
        auto pBackend = getBackend(index);
        if (!pBackend) {
            return QColor(230, 70, 210); // TODO: Constants
        }
        return m_pBackendContext->getBackendDynamicManager()->isBackendOnline(pBackend->getId()) ?
                   QColor(110, 240, 170) : QColor(240, 120, 150); // TODO: Constants
    }

    if (role > Qt::UserRole) {
        auto pBackend = getBackend(index);
        if (!pBackend) {
            return {};
        }
        switch (role)
        {
        case R_id:      return pBackend->getId().has_value() ? QString::fromStdString(pBackend->getId().value()) : QString();
        case R_address: return QString::fromStdString(pBackend->getFullAddress());
        case R_type:    return pBackend->getType();
        case R_name:    return QString::fromStdString(pBackend->getDisplayName());
        case R_status:  return m_pBackendContext->getBackendDynamicManager()->isBackendOnline(pBackend->getId());
        case R_backendPtr: return QVariant::fromValue(pBackend);
        }
    }

    return QVariant();
}

Qt::ItemFlags BackendTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) &~ Qt::ItemIsEditable;
}

void BackendTableModel::setBackendContext(AIManagerContext *pContext)
{
    beginResetModel();
    if (m_pBackendContext) {
        disconnect(m_pBackendContext, nullptr, this, nullptr);
    }
    m_pBackendContext = pContext;
    if (m_pBackendContext) {
        auto pBackManager = m_pBackendContext->getBackendServiceManager();
        m_backends = *pBackManager->getAllBackends();

        connect(pBackManager, &AIBackendServiceManager::sig_backendAdded,
                this, [this](auto pBackend){
                    auto lb = m_backends.lower_bound(pBackend);
                    auto insRow = std::distance(m_backends.begin(), lb);
                    beginInsertRows(QModelIndex(), insRow, insRow);
                    m_backends.insert(pBackend);
                    endInsertRows();
                });
        connect(pBackManager, &AIBackendServiceManager::sig_backendUpdated,
                this, [this](auto pBackend){
                    auto lb = m_backends.find(pBackend);
                    if (m_backends.end() == lb) {
                                return;
                    }
                    auto bckRow = std::distance(m_backends.begin(), lb);
                    emit dataChanged(createIndex(bckRow, 0), createIndex(bckRow, columnCount() - 1));
                });
        connect(pBackManager, &AIBackendServiceManager::sig_backendRemoved,
                this, [this](auto pBackend){
                    auto lb = m_backends.find(pBackend);
                    if (m_backends.end() == lb) {
                        return;
                    }
                    auto bckRow = std::distance(m_backends.begin(), lb);
                    beginRemoveRows(QModelIndex(), bckRow, bckRow);
                    m_backends.erase(pBackend);
                    endRemoveRows();
                });
    }
    endResetModel();
}

DBRecords::AIBackendInfoPtr BackendTableModel::getBackend(int row) const
{
    if (row >= rowCount() || row < 0) {
        return {};
    }
    auto sPos = m_backends.begin();
    std::advance(sPos, row);
    return *sPos;
}

DBRecords::AIBackendInfoPtr BackendTableModel::getBackend(const QModelIndex &idx) const
{
    return getBackend(idx.row());
}
