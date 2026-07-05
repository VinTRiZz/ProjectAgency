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
    if (parent.isValid() || !m_backends.isValid())
        return 0;
    return m_backends->size();
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
        case C_id:      return QString::fromStdString(pBackend->getId());
        case C_address: return QString::fromStdString(pBackend->getFullAddress());
        case C_type:    return pBackend->getType();
        case C_name:    return QString::fromStdString(pBackend->getDisplayName());
        case C_status:  return m_pBackendContext->getBackendDynamicManager()->isBackendOnline(pBackend->getId());
        }
    }

    return QVariant();
}

bool BackendTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
        return false;

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto pBackend = getBackend(index);
        if (!pBackend) {
            return {};
        }

        switch (index.column())
        {
        case C_address: return setData(index, value, R_address);
        case C_name:    return setData(index, value, R_name);
        case C_type:    return setData(index, value, R_type);
        }
        return false;
    }

    if (role > Qt::UserRole) {
        auto pBackend = getBackend(index);
        if (!pBackend) {
            return {};
        }
        bool isDataChanged = false;
        switch (role)
        {
        case C_address:
            isDataChanged = true;
            {
                auto addr = value.toString();
                auto addrParts = addr.split(":");
                if (addrParts.size() < 2 || addrParts[1].isEmpty()) { // No port
                    return false;
                }
                auto portValue = addrParts[1].toInt();
                if (portValue < 0 || portValue > 65535) { // Invalid port
                    return false;
                }
                pBackend->setIp(addrParts[0].toStdString());
                pBackend->setPort(portValue);
            }
            break;
        case C_type:
            isDataChanged = true;
            {
                auto devtype = DBRecords::AIBackendDeviceType(value.toInt());
                if (devtype >= DBRecords::SYS_Devtype_max || devtype < DBRecords::Default) { // Invalid type
                    return false;
                }
                pBackend->setType(devtype);
            }
            break;
        case C_name:
            isDataChanged = true;
            pBackend->setDisplayName(value.toString().toStdString());
            break;
        }
        if (isDataChanged) {
            emit dataChanged(index.siblingAtColumn(0), index.siblingAtColumn(columnCount() - 1), { Qt::DisplayRole });
        }
        return isDataChanged;
    }
    return false;
}

Qt::ItemFlags BackendTableModel::flags(const QModelIndex &index) const
{
    if (index.column() == C_id || index.column() == C_status) {
        return QAbstractItemModel::flags(index) &~ Qt::ItemIsEditable;
    }
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void BackendTableModel::setBackendContext(AIManagerContext *pContext)
{
    beginResetModel();
    // if (m_pBackendRegistry) {
    //     disconnect(m_pBackendRegistry, nullptr, this, nullptr);
    // }
    // m_pBackendContext = pContext;
    // if (m_pBackendRegistry) {
    //     connect(m_pBackendRegistry, &Web::ServerRegistry::serverAdded,
    //             this, [this](const auto& serverHdl){
    //                 // TODO: soft update, obviously
    //                 beginResetModel();
    //                 m_serversCache.insert(serverHdl);
    //                 endResetModel();
    //             });

    //     connect(m_pBackendRegistry, &Web::ServerRegistry::serverAboutToRemove,
    //             this, [this](const auto& serverHdl){
    //                 // TODO: soft update, obviously
    //                 beginResetModel();
    //                 m_serversCache.erase(serverHdl);
    //                 endResetModel();
    //             });
    //     m_serversCache = m_pBackendRegistry->getServers();
    // }
    endResetModel();
}

DBRecords::AIBackendInfoPtr BackendTableModel::getBackend(const QModelIndex &idx) const
{
    if (idx.row() >= rowCount() || idx.row() < 0) {
        return {};
    }
    auto sPos = m_backends->begin();
    std::advance(sPos, idx.row());
    return *sPos;
}
