#include "taskplan.hpp"

namespace Business
{

void TaskPlan::addAction(const ActionPtr &act)
{
    m_actionSet.emplace(act);
}

void TaskPlan::next()
{
    if (m_actionSet.empty()) {
        return;
    }
    m_actionSet.erase(m_actionSet.begin());
}

ActionPtr TaskPlan::getCurrentAction()
{
    if (m_actionSet.empty()) {
        return {};
    }
    return *m_actionSet.begin();
}

}