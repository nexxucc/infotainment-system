#pragma once

#include <chrono>
#include <string>
#include <nlohmann/json.hpp>

namespace infotainment {

enum class EventType {
    MediaPlayRequested,
    MediaPauseRequested,
    MediaStopRequested,
    MediaNextRequested,
    MediaPreviousRequested,
    VolumeChanged,
    BluetoothDeviceConnected,
    BluetoothDeviceDisconnected,
    IncomingCallReceived,
    CallAccepted,
    CallRejected,
    CallEnded,
    NavigationAlertStarted,
    NavigationAlertEnded,
    VehicleSpeedChanged,
    FuelLevelChanged,
    EngineTemperatureChanged,
    SettingChanged,
    NotificationQueued,
    NotificationDisplayed,
    ErrorRaised
};

inline std::string eventTypeToString(EventType type) {
    switch (type) {
        case EventType::MediaPlayRequested:         return "MediaPlayRequested";
        case EventType::MediaPauseRequested:         return "MediaPauseRequested";
        case EventType::MediaStopRequested:          return "MediaStopRequested";
        case EventType::MediaNextRequested:          return "MediaNextRequested";
        case EventType::MediaPreviousRequested:      return "MediaPreviousRequested";
        case EventType::VolumeChanged:               return "VolumeChanged";
        case EventType::BluetoothDeviceConnected:    return "BluetoothDeviceConnected";
        case EventType::BluetoothDeviceDisconnected: return "BluetoothDeviceDisconnected";
        case EventType::IncomingCallReceived:        return "IncomingCallReceived";
        case EventType::CallAccepted:                return "CallAccepted";
        case EventType::CallRejected:                return "CallRejected";
        case EventType::CallEnded:                   return "CallEnded";
        case EventType::NavigationAlertStarted:      return "NavigationAlertStarted";
        case EventType::NavigationAlertEnded:        return "NavigationAlertEnded";
        case EventType::VehicleSpeedChanged:         return "VehicleSpeedChanged";
        case EventType::FuelLevelChanged:            return "FuelLevelChanged";
        case EventType::EngineTemperatureChanged:    return "EngineTemperatureChanged";
        case EventType::SettingChanged:              return "SettingChanged";
        case EventType::NotificationQueued:          return "NotificationQueued";
        case EventType::NotificationDisplayed:       return "NotificationDisplayed";
        case EventType::ErrorRaised:                 return "ErrorRaised";
    }
    return "Unknown";
}

struct Event {
    EventType type;
    nlohmann::json data;
    std::string correlationId;
    std::chrono::system_clock::time_point timestamp;

    Event(EventType t, nlohmann::json d = {}, std::string corrId = "")
        : type(t)
        , data(std::move(d))
        , correlationId(std::move(corrId))
        , timestamp(std::chrono::system_clock::now()) {}

    nlohmann::json toJson() const {
        nlohmann::json j;
        j["type"] = eventTypeToString(type);
        j["data"] = data;
        j["correlation_id"] = correlationId;
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()) % 1000;
        std::tm tm{};
        gmtime_r(&time_t, &tm);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
        j["timestamp"] = std::string(buf) + "." +
            std::to_string(ms.count()) + "Z";
        return j;
    }
};

} // namespace infotainment
