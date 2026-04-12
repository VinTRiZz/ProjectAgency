#pragma once

#include <stdint.h>
#include <string>
#include <functional>
#include <memory>

namespace Business {

/**
 * @brief The Action class Executable action for plans
 * @note For better configurability, can be inherited
 */
class Action
{
public:
    Action();
    Action(std::function<bool()>&& doFunc);
    virtual ~Action();

    void setDebugEnabled(bool isEn);
    uint64_t getId() const;

    void setName(const std::string& actionNameString);
    std::string getName() const;

    void setPriority(uint64_t prior);
    uint64_t getPriority() const;

    void setAction(std::function<bool()>&& doFunc, std::function<bool()>&& undoFunc);
    bool execute() const;
    bool undo() const;

    bool operator <(Action& act) const;

private:
    bool m_debugEnabled {false};

    uint64_t m_id {};
    uint64_t m_priority {10000}; // Lower - cooler
    std::string m_name;

    std::function<bool()> m_doFunc;
    std::function<bool()> m_undoFunc;

    template <typename...Args>
    void logDebug(Args&&...args) const;
};
using ActionPtr = std::shared_ptr<Action>;

} // namespace Business
