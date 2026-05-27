#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace infotainment {

enum class MediaState { Stopped, Playing, Paused };
enum class BluetoothState { Disconnected, Pairing, Connected };
enum class CallState { Idle, Ringing, Active };
enum class NavigationState { Idle, AlertActive };
enum class ActiveScreen {
    Dashboard, Media, Bluetooth, Navigation,
    Telemetry, Settings, CallOverlay, ErrorModal
};

// --- Enum to/from string helpers ---

inline std::string mediaStateToString(MediaState s) {
    switch (s) {
        case MediaState::Stopped: return "stopped";
        case MediaState::Playing: return "playing";
        case MediaState::Paused:  return "paused";
    }
    return "unknown";
}

inline MediaState mediaStateFromString(const std::string& s) {
    if (s == "playing") return MediaState::Playing;
    if (s == "paused")  return MediaState::Paused;
    return MediaState::Stopped;
}

inline std::string bluetoothStateToString(BluetoothState s) {
    switch (s) {
        case BluetoothState::Disconnected: return "disconnected";
        case BluetoothState::Pairing:      return "pairing";
        case BluetoothState::Connected:    return "connected";
    }
    return "unknown";
}

inline BluetoothState bluetoothStateFromString(const std::string& s) {
    if (s == "pairing")   return BluetoothState::Pairing;
    if (s == "connected") return BluetoothState::Connected;
    return BluetoothState::Disconnected;
}

inline std::string callStateToString(CallState s) {
    switch (s) {
        case CallState::Idle:    return "idle";
        case CallState::Ringing: return "ringing";
        case CallState::Active:  return "active";
    }
    return "unknown";
}

inline CallState callStateFromString(const std::string& s) {
    if (s == "ringing") return CallState::Ringing;
    if (s == "active")  return CallState::Active;
    return CallState::Idle;
}

inline std::string navigationStateToString(NavigationState s) {
    switch (s) {
        case NavigationState::Idle:        return "idle";
        case NavigationState::AlertActive: return "alert_active";
    }
    return "unknown";
}

inline NavigationState navigationStateFromString(const std::string& s) {
    if (s == "alert_active") return NavigationState::AlertActive;
    return NavigationState::Idle;
}

inline std::string activeScreenToString(ActiveScreen s) {
    switch (s) {
        case ActiveScreen::Dashboard:    return "dashboard";
        case ActiveScreen::Media:        return "media";
        case ActiveScreen::Bluetooth:    return "bluetooth";
        case ActiveScreen::Navigation:   return "navigation";
        case ActiveScreen::Telemetry:    return "telemetry";
        case ActiveScreen::Settings:     return "settings";
        case ActiveScreen::CallOverlay:  return "call_overlay";
        case ActiveScreen::ErrorModal:   return "error_modal";
    }
    return "unknown";
}

inline ActiveScreen activeScreenFromString(const std::string& s) {
    if (s == "media")        return ActiveScreen::Media;
    if (s == "bluetooth")    return ActiveScreen::Bluetooth;
    if (s == "navigation")   return ActiveScreen::Navigation;
    if (s == "telemetry")    return ActiveScreen::Telemetry;
    if (s == "settings")     return ActiveScreen::Settings;
    if (s == "call_overlay") return ActiveScreen::CallOverlay;
    if (s == "error_modal")  return ActiveScreen::ErrorModal;
    return ActiveScreen::Dashboard;
}

// --- Application State ---

struct AppState {
    MediaState mediaState = MediaState::Stopped;
    std::string track;
    int volume = 50;

    BluetoothState bluetoothState = BluetoothState::Disconnected;
    std::string connectedDevice;

    CallState callState = CallState::Idle;
    std::string currentCaller;

    NavigationState navigationState = NavigationState::Idle;
    std::string navigationMessage;

    ActiveScreen activeScreen = ActiveScreen::Dashboard;

    int vehicleSpeed = 0;
    int fuelLevel = 100;
    int engineTemperature = 20;

    int notificationsPending = 0;
    std::string lastError;

    nlohmann::json toJson() const {
        nlohmann::json j;
        j["media_state"] = mediaStateToString(mediaState);
        j["track"] = track;
        j["volume"] = volume;
        j["bluetooth_state"] = bluetoothStateToString(bluetoothState);
        j["connected_device"] = connectedDevice.empty() ? nlohmann::json(nullptr) : nlohmann::json(connectedDevice);
        j["call_state"] = callStateToString(callState);
        j["caller"] = currentCaller.empty() ? nlohmann::json(nullptr) : nlohmann::json(currentCaller);
        j["navigation_state"] = navigationStateToString(navigationState);
        j["navigation_message"] = navigationMessage.empty() ? nlohmann::json(nullptr) : nlohmann::json(navigationMessage);
        j["active_screen"] = activeScreenToString(activeScreen);
        j["vehicle_speed"] = vehicleSpeed;
        j["fuel_level"] = fuelLevel;
        j["engine_temperature"] = engineTemperature;
        j["notifications_pending"] = notificationsPending;
        j["last_error"] = lastError.empty() ? nlohmann::json(nullptr) : nlohmann::json(lastError);
        return j;
    }

    void reset() {
        *this = AppState{};
    }
};

} // namespace infotainment
