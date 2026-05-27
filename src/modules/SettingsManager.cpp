#include "SettingsManager.h"
#include "../core/EventBus.h"
#include "../core/StateManager.h"
#include "../core/Logger.h"

namespace infotainment {

SettingsManager::SettingsManager(EventBus& eventBus, StateManager& stateManager, Logger& logger)
    : eventBus_(eventBus), stateManager_(stateManager), logger_(logger) {
    // Default settings
    settings_["theme"] = "dark";
    settings_["language"] = "en";
    settings_["units"] = "metric";
    settings_["brightness"] = "80";
    settings_["auto_connect_bluetooth"] = "true";
}

CommandResult SettingsManager::changeSetting(const std::string& key, const std::string& value,
                                              const std::string& correlationId) {
    if (key.empty()) {
        return CommandResult::error("MISSING_FIELD", "Setting key is required");
    }
    if (value.empty()) {
        return CommandResult::error("MISSING_FIELD", "Setting value is required");
    }

    auto beforeState = stateManager_.toJson();
    std::string oldValue = settings_.count(key) ? settings_[key] : "";
    settings_[key] = value;

    auto afterState = stateManager_.toJson();
    logger_.info("SettingsManager", "SettingChanged",
                 key + ": " + oldValue + " -> " + value,
                 beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::SettingChanged,
                           {{"key", key}, {"old_value", oldValue}, {"new_value", value}},
                           correlationId));

    return CommandResult::ok("Setting '" + key + "' changed to '" + value + "'");
}

std::map<std::string, std::string> SettingsManager::getAllSettings() const {
    return settings_;
}

std::string SettingsManager::getSetting(const std::string& key) const {
    auto it = settings_.find(key);
    return it != settings_.end() ? it->second : "";
}

} // namespace infotainment
