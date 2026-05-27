#pragma once

#include "MediaManager.h" // for CommandResult
#include <string>
#include <map>

namespace infotainment {

class EventBus;
class StateManager;
class Logger;

class SettingsManager {
public:
    SettingsManager(EventBus& eventBus, StateManager& stateManager, Logger& logger);

    CommandResult changeSetting(const std::string& key, const std::string& value,
                                const std::string& correlationId = "");
    std::map<std::string, std::string> getAllSettings() const;
    std::string getSetting(const std::string& key) const;

private:
    EventBus& eventBus_;
    StateManager& stateManager_;
    Logger& logger_;
    std::map<std::string, std::string> settings_;
};

} // namespace infotainment
