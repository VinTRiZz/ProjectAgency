#pragma once

#include "action.hpp"

#include <set>

namespace Business
{

/**
 * @brief The TaskPlan class Plan for AI to execute
 */
class TaskPlan
{
public:
    void addAction(const ActionPtr& act);

    void next();
    ActionPtr getCurrentAction();

private:
    struct ActionPtrComparator
    {
        bool operator ()(const ActionPtr& pAl, const ActionPtr& pAr) const {
            return *pAl < *pAr;
        }
    };
    std::set<ActionPtr, ActionPtrComparator> m_actionSet;
};

}