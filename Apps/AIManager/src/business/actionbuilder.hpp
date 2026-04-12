#pragma once

#include "business/action.hpp"

#include <list>

namespace Business {

/**
 * @brief The ActionBuilder class Action builder with common action configuration providing
 */
class ActionBuilder
{
public:
    template <typename ActionT, typename...Args>
    std::shared_ptr<ActionT> createAction(Args&&...args) {
        auto pAction = std::make_shared<ActionT>(args...);
        if (pAction) {
            initAction(pAction);
        }
        return pAction;
    }

    ActionPtr createDefaultAction(const std::string& actionName);
    std::list<std::string> getAvailableActions();

private:

    // Common action initializer
    void initAction(const ActionPtr& pAction);
};

} // namespace Business
