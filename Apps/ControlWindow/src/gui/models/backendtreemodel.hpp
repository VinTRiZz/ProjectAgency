#pragma once

#include <QObject>

#include <optional>

#include <Components/CustomQt/Models/TreeGroupingProxyModel.h>

#include <ProjectAgency/DB/AIBackendInfo.h>

class BackendTreeModel : public QtCustom::Models::TreeGroupingProxyModel
{
public:
    explicit BackendTreeModel(QObject *parent = nullptr);

    // Defines grouping levels
    enum GroupingRule : int
    {
        GR_disabled = 0,
        GR_type,
        GR_name,
        GR_ip,
        GR_status,
        GR_isOnline,
    };

    void setGroupingRule(uint8_t level, GroupingRule grRule);
    void removeGroup(uint8_t level);
    std::optional<GroupingRule> getGroupingRule(uint8_t level) const;
    uint8_t getMaxGroupLevel() const;

private:
    std::map<uint8_t, GroupingRule> m_groupingRules;

    QString getGroupName(const DBRecords::AIBackendInfoPtr& pBackend, GroupingRule grRule) const;

    // TreeGroupingProxyModel interface
protected:
    GroupKey_t getGroup(int sourceModelRow) const override;
    GroupKey_t getParentGroup(GroupKey_t groupKey) const override;
    uint getGroupHash(const GroupKey_t &groupKey) const override;
    bool canMergeGroups(const GroupKey_t &lgk, const GroupKey_t &rgk) const override;
    QVariant getGroupData(GroupKey_t groupKey, int column, int role) const override;
    bool setGroupData(GroupKey_t groupKey, int column, const QVariant &value, int role) override;
};
