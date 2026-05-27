#pragma once

#include "../core/AppState.h"
#include <string>
#include <nlohmann/json.hpp>

namespace infotainment {

class EventBus;
class StateManager;
class Logger;

/// Result type for manager operations
struct CommandResult {
    bool success;
    std::string errorCode;
    std::string message;

    static CommandResult ok(const std::string& msg = "OK") {
        return {true, "", msg};
    }

    static CommandResult error(const std::string& code, const std::string& msg) {
        return {false, code, msg};
    }
};

class MediaManager {
public:
    MediaManager(EventBus& eventBus, StateManager& stateManager, Logger& logger);

    CommandResult play(const std::string& track, const std::string& correlationId = "");
    CommandResult pause(const std::string& correlationId = "");
    CommandResult stop(const std::string& correlationId = "");
    CommandResult next(const std::string& correlationId = "");
    CommandResult previous(const std::string& correlationId = "");
    CommandResult setVolume(int volume, const std::string& correlationId = "");

private:
    EventBus& eventBus_;
    StateManager& stateManager_;
    Logger& logger_;
};

} // namespace infotainment
