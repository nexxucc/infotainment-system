#pragma once

#include "../core/AppState.h"
#include "MediaManager.h" // for CommandResult
#include <string>

namespace infotainment {

class EventBus;
class StateManager;
class Logger;

class BluetoothManager {
public:
    BluetoothManager(EventBus& eventBus, StateManager& stateManager, Logger& logger);

    CommandResult connect(const std::string& device, const std::string& correlationId = "");
    CommandResult disconnect(const std::string& correlationId = "");
    CommandResult incomingCall(const std::string& caller, const std::string& correlationId = "");
    CommandResult acceptCall(const std::string& correlationId = "");
    CommandResult rejectCall(const std::string& correlationId = "");
    CommandResult endCall(const std::string& correlationId = "");

private:
    EventBus& eventBus_;
    StateManager& stateManager_;
    Logger& logger_;
};

} // namespace infotainment
