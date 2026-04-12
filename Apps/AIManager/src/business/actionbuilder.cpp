#include "actionbuilder.hpp"

namespace Business {

ActionPtr ActionBuilder::createDefaultAction(const std::string &actionName)
{
    auto pAction = std::make_shared<Action>(); // TODO: Get from list
    if (pAction) {
        initAction(pAction);
    }
    return pAction;
}

std::list<std::string> ActionBuilder::getAvailableActions()
{
    return {
        // TODO: Setup action list
    };
}

void ActionBuilder::initAction(const ActionPtr &pAction)
{
    // TODO: Init actions
}

} // namespace Business
