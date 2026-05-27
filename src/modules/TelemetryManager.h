#pragma once

#include "MediaManager.h" // for CommandResult
#include <string>

namespace infotainment {

class EventBus;
class StateManager;
class Logger;

class TelemetryManager {
public:
    TelemetryManager(EventBus& eventBus, StateManager& stateManager, Logger& logger);

    CommandResult updateSpeed(int speed, const std::string& correlationId = "");
    CommandResult updateFuel(int fuel, const std::string& correlationId = "");
    CommandResult updateTemperature(int temp, const std::string& correlationId = "");
    CommandResult updateAll(int speed, int fuel, int temp, const std::string& correlationId = "");

private:
    EventBus& eventBus_;
    StateManager& stateManager_;
    Logger& logger_;
};

} // namespace infotainment
