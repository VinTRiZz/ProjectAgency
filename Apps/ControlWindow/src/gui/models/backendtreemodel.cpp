#include "backendtreemodel.hpp"

#include "backendtablemodel.hpp"

namespace {
using levelDescriptor_t = std::pair<uint8_t, DBRecords::AIBackendInfoPtr>;
}

Q_DECLARE_METATYPE(levelDescriptor_t);

BackendTreeModel::BackendTreeModel(QObject *parent)
    : QtCustom::Models::TreeGroupingProxyModel{parent}
{
    setGroupingRule(0, GroupingRule::GR_type);
}

void BackendTreeModel::setGroupingRule(uint8_t level, GroupingRule grRule)
{
    m_groupingRules[level] = grRule;
    resetTree();
}

void BackendTreeModel::removeGroup(uint8_t level)
{
    m_groupingRules.erase(level);
    resetTree();
}

uint8_t BackendTreeModel::getMaxGroupLevel() const
{
    if (m_groupingRules.empty()) {
        return {};
    }
    // dirty, but optimal :)
    auto it = m_groupingRules.end();
    std::advance(it, -1);
    return it->first;
}

std::optional<BackendTreeModel::GroupingRule> BackendTreeModel::getGroupingRule(uint8_t level) const
{
    if (m_groupingRules.count(level)) {
        return m_groupingRules.at(level);
    }
    return {};
}

QString BackendTreeModel::getGroupName(const DBRecords::AIBackendInfoPtr &pBackend, GroupingRule grRule) const
{
    switch (grRule)
    {
    case GR_disabled:
        return "No group";
    case GR_type:
        return QString::fromStdString(pBackend->getTypeString());
    case GR_name:
        return QString::fromStdString(pBackend->getDisplayName());
    case GR_ip:
        return QString::fromStdString(pBackend->getIp());
    case GR_status:
        return "Unknown status"; // TODO: Implement
    case GR_isOnline:
        return "Offline"; // TODO: Implement
    }
    return {};
}

QtCustom::Models::TreeGroupingProxyModel::GroupKey_t BackendTreeModel::getGroup(int sourceModelRow) const
{
    auto pSourceModel = static_cast<BackendTableModel*>(sourceModel());
    auto pBackend = pSourceModel->getBackend(sourceModelRow);
    if (!pBackend) { return {}; } // Unknown issue actually
    if (!m_groupingRules.count(0)) { return {}; } // No grouping enabled
    return QVariant::fromValue(levelDescriptor_t(0, pBackend));
}

QtCustom::Models::TreeGroupingProxyModel::GroupKey_t BackendTreeModel::getParentGroup(const GroupKey_t& groupKey) const
{
    auto backendDescr = groupKey.value<levelDescriptor_t>();
    if (!backendDescr.second) { return {}; }
    if (m_groupingRules.count(backendDescr.first + 1)) {
        backendDescr.first++;
        return QVariant::fromValue(backendDescr);
    }
    return {};
}

uint BackendTreeModel::getGroupHash(const GroupKey_t &groupKey) const
{
    auto backendDescr = groupKey.value<levelDescriptor_t>();
    if (!backendDescr.second) { return {}; }
    return qHash(getGroupName(backendDescr.second, m_groupingRules.at(backendDescr.first)), qGlobalQHashSeed());
}

bool BackendTreeModel::canMergeGroups(const GroupKey_t &lgk, const GroupKey_t &rgk) const
{
    // Reimplemented because of optimised logic
    auto lgv = lgk.value<levelDescriptor_t>();
    auto rgv = rgk.value<levelDescriptor_t>();
    if (!lgv.second || !rgv.second) { return false; }
    return (getGroupName(lgv.second, m_groupingRules.at(lgv.first)) ==
            getGroupName(rgv.second, m_groupingRules.at(rgv.first)));
}

QVariant BackendTreeModel::getGroupData(GroupKey_t groupKey, int column, int role) const
{
    auto backendDescr = groupKey.value<levelDescriptor_t>();
    if (!backendDescr.second) { return {}; }
    if (role != Qt::DisplayRole || column != treeColumn()) { return TreeGroupingProxyModel::getGroupData(groupKey, column, role); }
    return getGroupName(backendDescr.second, m_groupingRules.at(backendDescr.first));
}

bool BackendTreeModel::setGroupData(GroupKey_t groupKey, int column, const QVariant &value, int role)
{
    return false; // No group editing
}
