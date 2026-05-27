#pragma once

#include "../core/AppState.h"
#include "MediaManager.h" // for CommandResult
#include <string>

namespace infotainment {

class EventBus;
class StateManager;
class Logger;

class NavigationManager {
public:
    NavigationManager(EventBus& eventBus, StateManager& stateManager, Logger& logger);

    CommandResult startAlert(const std::string& message, const std::string& priority,
                             const std::string& correlationId = "");
    CommandResult endAlert(const std::string& correlationId = "");

private:
    EventBus& eventBus_;
    StateManager& stateManager_;
    Logger& logger_;
};

} // namespace infotainment
