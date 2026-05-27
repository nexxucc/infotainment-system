#include "StateManager.h"

namespace infotainment {

StateManager::StateManager(QObject* parent) : QObject(parent) {}

AppState StateManager::getState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

nlohmann::json StateManager::toJson() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.toJson();
}

// --- Getters ---

MediaState StateManager::getMediaState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.mediaState;
}

std::string StateManager::getTrack() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.track;
}

int StateManager::getVolume() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.volume;
}

BluetoothState StateManager::getBluetoothState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.bluetoothState;
}

std::string StateManager::getConnectedDevice() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.connectedDevice;
}

CallState StateManager::getCallState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.callState;
}

std::string StateManager::getCurrentCaller() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.currentCaller;
}

NavigationState StateManager::getNavigationState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.navigationState;
}

ActiveScreen StateManager::getActiveScreen() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.activeScreen;
}

int StateManager::getVehicleSpeed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.vehicleSpeed;
}

int StateManager::getFuelLevel() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.fuelLevel;
}

int StateManager::getEngineTemperature() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.engineTemperature;
}

int StateManager::getNotificationsPending() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.notificationsPending;
}

std::string StateManager::getLastError() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_.lastError;
}

// --- Setters ---

bool StateManager::setMediaState(MediaState state) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.mediaState = state;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setTrack(const std::string& track) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.track = track;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setVolume(int volume) {
    if (volume < 0 || volume > 100) return false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.volume = volume;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setBluetoothState(BluetoothState state) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.bluetoothState = state;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setConnectedDevice(const std::string& device) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.connectedDevice = device;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setCallState(CallState state) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.callState = state;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setCurrentCaller(const std::string& caller) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.currentCaller = caller;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setNavigationState(NavigationState state) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.navigationState = state;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setNavigationMessage(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.navigationMessage = message;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setActiveScreen(ActiveScreen screen) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.activeScreen = screen;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setVehicleSpeed(int speed) {
    if (speed < 0 || speed > 240) return false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.vehicleSpeed = speed;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setFuelLevel(int level) {
    if (level < 0 || level > 100) return false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.fuelLevel = level;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setEngineTemperature(int temp) {
    if (temp < -20 || temp > 140) return false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.engineTemperature = temp;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setNotificationsPending(int count) {
    if (count < 0) return false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.notificationsPending = count;
    }
    emit stateChanged();
    return true;
}

bool StateManager::setLastError(const std::string& error) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.lastError = error;
    }
    emit stateChanged();
    return true;
}

void StateManager::reset() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.reset();
    }
    emit stateChanged();
}

} // namespace infotainment
