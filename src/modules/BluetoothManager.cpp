#include "BluetoothManager.h"
#include "../core/EventBus.h"
#include "../core/StateManager.h"
#include "../core/Logger.h"

namespace infotainment {

BluetoothManager::BluetoothManager(EventBus& eventBus, StateManager& stateManager, Logger& logger)
    : eventBus_(eventBus), stateManager_(stateManager), logger_(logger) {}

CommandResult BluetoothManager::connect(const std::string& device, const std::string& correlationId) {
    if (device.empty()) {
        return CommandResult::error("MISSING_FIELD", "Device name is required");
    }

    auto btState = stateManager_.getBluetoothState();
    if (btState == BluetoothState::Connected) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "Already connected to a device. Disconnect first.");
    }

    auto beforeState = stateManager_.toJson();

    // Transition through Pairing to Connected (simulated instant pairing)
    stateManager_.setBluetoothState(BluetoothState::Pairing);
    stateManager_.setBluetoothState(BluetoothState::Connected);
    stateManager_.setConnectedDevice(device);

    auto afterState = stateManager_.toJson();
    logger_.info("BluetoothManager", "BluetoothDeviceConnected",
                 "Connected to: " + device, beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::BluetoothDeviceConnected, {{"device", device}}, correlationId));

    return CommandResult::ok("Connected to " + device);
}

CommandResult BluetoothManager::disconnect(const std::string& correlationId) {
    auto btState = stateManager_.getBluetoothState();
    if (btState == BluetoothState::Disconnected) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "No device connected");
    }

    auto beforeState = stateManager_.toJson();
    auto device = stateManager_.getConnectedDevice();

    // If there's an active call, end it first
    auto callState = stateManager_.getCallState();
    if (callState != CallState::Idle) {
        stateManager_.setCallState(CallState::Idle);
        stateManager_.setCurrentCaller("");
        if (stateManager_.getActiveScreen() == ActiveScreen::CallOverlay) {
            stateManager_.setActiveScreen(ActiveScreen::Dashboard);
        }
        eventBus_.publish(Event(EventType::CallEnded, {}, correlationId));
    }

    stateManager_.setBluetoothState(BluetoothState::Disconnected);
    stateManager_.setConnectedDevice("");

    auto afterState = stateManager_.toJson();
    logger_.info("BluetoothManager", "BluetoothDeviceDisconnected",
                 "Disconnected from: " + device, beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::BluetoothDeviceDisconnected, {{"device", device}}, correlationId));

    return CommandResult::ok("Disconnected from " + device);
}

CommandResult BluetoothManager::incomingCall(const std::string& caller, const std::string& correlationId) {
    if (caller.empty()) {
        return CommandResult::error("MISSING_FIELD", "Caller is required");
    }

    auto btState = stateManager_.getBluetoothState();
    if (btState != BluetoothState::Connected) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "Cannot receive call: no Bluetooth device connected");
    }

    auto callState = stateManager_.getCallState();
    if (callState != CallState::Idle) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "Cannot receive call: already in a call");
    }

    auto beforeState = stateManager_.toJson();

    // Incoming call pauses playing media (spec requirement)
    if (stateManager_.getMediaState() == MediaState::Playing) {
        stateManager_.setMediaState(MediaState::Paused);
        eventBus_.publish(Event(EventType::MediaPauseRequested, {{"reason", "incoming_call"}}, correlationId));
    }

    stateManager_.setCallState(CallState::Ringing);
    stateManager_.setCurrentCaller(caller);
    stateManager_.setActiveScreen(ActiveScreen::CallOverlay);

    auto afterState = stateManager_.toJson();
    logger_.info("BluetoothManager", "IncomingCallReceived",
                 "Incoming call from: " + caller, beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::IncomingCallReceived, {{"caller", caller}}, correlationId));

    return CommandResult::ok("Incoming call from " + caller);
}

CommandResult BluetoothManager::acceptCall(const std::string& correlationId) {
    auto callState = stateManager_.getCallState();
    if (callState != CallState::Ringing) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "No ringing call to accept");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setCallState(CallState::Active);

    auto afterState = stateManager_.toJson();
    logger_.info("BluetoothManager", "CallAccepted",
                 "Call accepted", beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::CallAccepted, {}, correlationId));

    return CommandResult::ok("Call accepted");
}

CommandResult BluetoothManager::rejectCall(const std::string& correlationId) {
    auto callState = stateManager_.getCallState();
    if (callState != CallState::Ringing) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "No ringing call to reject");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setCallState(CallState::Idle);
    stateManager_.setCurrentCaller("");
    stateManager_.setActiveScreen(ActiveScreen::Dashboard);

    auto afterState = stateManager_.toJson();
    logger_.info("BluetoothManager", "CallRejected",
                 "Call rejected", beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::CallRejected, {}, correlationId));

    return CommandResult::ok("Call rejected");
}

CommandResult BluetoothManager::endCall(const std::string& correlationId) {
    auto callState = stateManager_.getCallState();
    if (callState == CallState::Idle) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "No active call to end");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setCallState(CallState::Idle);
    stateManager_.setCurrentCaller("");
    stateManager_.setActiveScreen(ActiveScreen::Dashboard);

    auto afterState = stateManager_.toJson();
    logger_.info("BluetoothManager", "CallEnded",
                 "Call ended", beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::CallEnded, {}, correlationId));

    return CommandResult::ok("Call ended");
}

} // namespace infotainment
