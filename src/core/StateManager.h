#pragma once

#include "AppState.h"
#include <QObject>
#include <mutex>
#include <nlohmann/json.hpp>

namespace infotainment {

/// Centralized application state manager with thread-safe access and Qt signal notifications.
class StateManager : public QObject {
    Q_OBJECT

public:
    explicit StateManager(QObject* parent = nullptr);

    // --- Getters ---
    AppState getState() const;
    nlohmann::json toJson() const;

    MediaState getMediaState() const;
    std::string getTrack() const;
    int getVolume() const;

    BluetoothState getBluetoothState() const;
    std::string getConnectedDevice() const;

    CallState getCallState() const;
    std::string getCurrentCaller() const;

    NavigationState getNavigationState() const;
    ActiveScreen getActiveScreen() const;

    int getVehicleSpeed() const;
    int getFuelLevel() const;
    int getEngineTemperature() const;
    int getNotificationsPending() const;
    std::string getLastError() const;

    // --- Setters (return false if validation fails) ---
    bool setMediaState(MediaState state);
    bool setTrack(const std::string& track);
    bool setVolume(int volume);

    bool setBluetoothState(BluetoothState state);
    bool setConnectedDevice(const std::string& device);

    bool setCallState(CallState state);
    bool setCurrentCaller(const std::string& caller);

    bool setNavigationState(NavigationState state);
    bool setNavigationMessage(const std::string& message);
    bool setActiveScreen(ActiveScreen screen);

    bool setVehicleSpeed(int speed);
    bool setFuelLevel(int level);
    bool setEngineTemperature(int temp);
    bool setNotificationsPending(int count);
    bool setLastError(const std::string& error);

    /// Reset all state to defaults.
    void reset();

signals:
    void stateChanged();

private:
    mutable std::mutex mutex_;
    AppState state_;
};

} // namespace infotainment
