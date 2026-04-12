#include "action.hpp"

#include <Components/Logger/Logger.h>

namespace Business {

static uint64_t currentActionId {0};
static uint64_t aliveActionCount {0};

template <typename...Args>
void Action::logDebug(Args&&...args) const {
    if (!m_debugEnabled) {
        return;
    }
    COMPLOG_INFO("[DEBUG: ACTION", m_id, "]", args...);
}

Action::Action()
{
    m_id = ++currentActionId;
    ++aliveActionCount;
}

Action::Action(std::function<bool ()> &&doFunc) :
    m_doFunc {doFunc}
{
    m_id = ++currentActionId;
    ++aliveActionCount;
}

Action::~Action()
{
    --aliveActionCount;
    if (aliveActionCount == 0) {
        currentActionId = 0;
    }
}

void Action::setDebugEnabled(bool isEn)
{
    m_debugEnabled = isEn;
}

uint64_t Action::getId() const
{
    return m_id;
}

void Action::setName(const std::string &actionNameString)
{
    m_name = actionNameString;
}

std::string Action::getName() const
{
    return m_name;
}

void Action::setPriority(uint64_t prior)
{
    m_priority = prior;
}

uint64_t Action::getPriority() const
{
    return m_priority;
}

void Action::setAction(std::function<bool ()> &&doFunc, std::function<bool ()> &&undoFunc)
{
    m_doFunc = std::move(doFunc);
    m_undoFunc = std::move(undoFunc);
}

bool Action::execute() const
{
    if (!m_doFunc) {
        logDebug("Exec failed: undefined function");
        return false;
    }
    logDebug("Exec start");
    auto execRes = m_doFunc();
    logDebug("Exec result:", execRes);
    return execRes;
}

bool Action::undo() const
{
    if (!m_undoFunc) {
        logDebug("Undo failed: undefined function");
        return false;
    }
    logDebug("Undo start");
    auto execRes = m_undoFunc();
    logDebug("Undo result:", execRes);
    return execRes;
}

bool Action::operator <(Action& act) const {
    if (m_priority == act.m_priority) {
        return m_id < act.m_id;
    }
    return m_priority < act.m_priority;
}

} // namespace Business
