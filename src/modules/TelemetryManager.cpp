#include "TelemetryManager.h"
#include "../core/EventBus.h"
#include "../core/StateManager.h"
#include "../core/Logger.h"

namespace infotainment {

TelemetryManager::TelemetryManager(EventBus& eventBus, StateManager& stateManager, Logger& logger)
    : eventBus_(eventBus), stateManager_(stateManager), logger_(logger) {}

CommandResult TelemetryManager::updateSpeed(int speed, const std::string& correlationId) {
    if (speed < 0 || speed > 240) {
        return CommandResult::error("VALUE_OUT_OF_RANGE",
                                   "Speed must be between 0 and 240 km/h");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setVehicleSpeed(speed);
    auto afterState = stateManager_.toJson();

    logger_.info("TelemetryManager", "VehicleSpeedChanged",
                 "Speed updated to " + std::to_string(speed),
                 beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::VehicleSpeedChanged, {{"speed", speed}}, correlationId));

    return CommandResult::ok("Speed updated to " + std::to_string(speed));
}

CommandResult TelemetryManager::updateFuel(int fuel, const std::string& correlationId) {
    if (fuel < 0 || fuel > 100) {
        return CommandResult::error("VALUE_OUT_OF_RANGE",
                                   "Fuel level must be between 0 and 100%");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setFuelLevel(fuel);
    auto afterState = stateManager_.toJson();

    logger_.info("TelemetryManager", "FuelLevelChanged",
                 "Fuel updated to " + std::to_string(fuel),
                 beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::FuelLevelChanged, {{"fuel", fuel}}, correlationId));

    return CommandResult::ok("Fuel updated to " + std::to_string(fuel));
}

CommandResult TelemetryManager::updateTemperature(int temp, const std::string& correlationId) {
    if (temp < -20 || temp > 140) {
        return CommandResult::error("VALUE_OUT_OF_RANGE",
                                   "Temperature must be between -20 and 140°C");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setEngineTemperature(temp);
    auto afterState = stateManager_.toJson();

    logger_.info("TelemetryManager", "EngineTemperatureChanged",
                 "Temperature updated to " + std::to_string(temp),
                 beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::EngineTemperatureChanged, {{"temperature", temp}}, correlationId));

    return CommandResult::ok("Temperature updated to " + std::to_string(temp));
}

CommandResult TelemetryManager::updateAll(int speed, int fuel, int temp, const std::string& correlationId) {
    if (speed < 0 || speed > 240) {
        return CommandResult::error("VALUE_OUT_OF_RANGE", "Speed must be between 0 and 240");
    }
    if (fuel < 0 || fuel > 100) {
        return CommandResult::error("VALUE_OUT_OF_RANGE", "Fuel must be between 0 and 100");
    }
    if (temp < -20 || temp > 140) {
        return CommandResult::error("VALUE_OUT_OF_RANGE", "Temperature must be between -20 and 140");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setVehicleSpeed(speed);
    stateManager_.setFuelLevel(fuel);
    stateManager_.setEngineTemperature(temp);
    auto afterState = stateManager_.toJson();

    logger_.info("TelemetryManager", "TelemetryUpdated",
                 "Telemetry updated", beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::VehicleSpeedChanged, {{"speed", speed}}, correlationId));
    eventBus_.publish(Event(EventType::FuelLevelChanged, {{"fuel", fuel}}, correlationId));
    eventBus_.publish(Event(EventType::EngineTemperatureChanged, {{"temperature", temp}}, correlationId));

    return CommandResult::ok("Telemetry updated");
}

} // namespace infotainment
