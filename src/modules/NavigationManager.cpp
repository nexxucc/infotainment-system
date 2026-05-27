#include "NavigationManager.h"
#include "../core/EventBus.h"
#include "../core/StateManager.h"
#include "../core/Logger.h"

namespace infotainment {

NavigationManager::NavigationManager(EventBus& eventBus, StateManager& stateManager, Logger& logger)
    : eventBus_(eventBus), stateManager_(stateManager), logger_(logger) {}

CommandResult NavigationManager::startAlert(const std::string& message, const std::string& priority,
                                            const std::string& correlationId) {
    if (message.empty()) {
        return CommandResult::error("MISSING_FIELD", "Navigation message is required");
    }

    auto beforeState = stateManager_.toJson();

    stateManager_.setNavigationState(NavigationState::AlertActive);
    stateManager_.setNavigationMessage(message);

    // Navigation alert must NOT override call overlay (spec requirement)
    auto callState = stateManager_.getCallState();
    if (callState == CallState::Idle) {
        stateManager_.setActiveScreen(ActiveScreen::Navigation);
    }
    // If in a call, navigation screen change is suppressed but alert is still active

    auto afterState = stateManager_.toJson();
    logger_.info("NavigationManager", "NavigationAlertStarted",
                 "Navigation alert: " + message,
                 beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::NavigationAlertStarted,
                           {{"message", message}, {"priority", priority}}, correlationId));

    return CommandResult::ok("Navigation alert started: " + message);
}

CommandResult NavigationManager::endAlert(const std::string& correlationId) {
    auto navState = stateManager_.getNavigationState();
    if (navState != NavigationState::AlertActive) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "No active navigation alert");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setNavigationState(NavigationState::Idle);
    stateManager_.setNavigationMessage("");

    // Return to dashboard if we were on navigation screen
    if (stateManager_.getActiveScreen() == ActiveScreen::Navigation) {
        stateManager_.setActiveScreen(ActiveScreen::Dashboard);
    }

    auto afterState = stateManager_.toJson();
    logger_.info("NavigationManager", "NavigationAlertEnded",
                 "Navigation alert ended", beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::NavigationAlertEnded, {}, correlationId));

    return CommandResult::ok("Navigation alert ended");
}

} // namespace infotainment
