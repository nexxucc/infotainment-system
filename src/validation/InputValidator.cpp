#include "InputValidator.h"
#include <set>

namespace infotainment {

static const std::set<std::string> kKnownActions = {
    "play_media", "pause_media", "stop_media", "next_media", "previous_media",
    "set_volume", "connect_bluetooth", "disconnect_bluetooth",
    "incoming_call", "accept_call", "reject_call", "end_call",
    "start_navigation_alert", "end_navigation_alert",
    "update_telemetry", "change_setting", "switch_screen",
    "queue_notification", "display_notification",
    "get_state", "get_recent_events", "reset_state"
};

CommandResult InputValidator::validateCommandStructure(const nlohmann::json& cmd) {
    if (!cmd.is_object()) {
        return CommandResult::error("INVALID_JSON", "Command must be a JSON object");
    }

    if (!cmd.contains("id")) {
        return CommandResult::error("MISSING_FIELD", "Missing required field: id");
    }

    if (!cmd.contains("action")) {
        return CommandResult::error("MISSING_FIELD", "Missing required field: action");
    }

    if (!cmd["action"].is_string()) {
        return CommandResult::error("INVALID_FIELD_TYPE", "Field 'action' must be a string");
    }

    if (!cmd.contains("params")) {
        return CommandResult::error("MISSING_FIELD", "Missing required field: params");
    }

    if (!cmd["params"].is_object()) {
        return CommandResult::error("INVALID_FIELD_TYPE", "Field 'params' must be an object");
    }

    std::string action = cmd["action"].get<std::string>();
    if (kKnownActions.find(action) == kKnownActions.end()) {
        return CommandResult::error("UNKNOWN_ACTION", "Unknown action: " + action);
    }

    return CommandResult::ok();
}

CommandResult InputValidator::validateParams(const std::string& action, const nlohmann::json& params) {
    if (action == "play_media") {
        return requireStringField(params, "track");
    }
    if (action == "set_volume") {
        return requireIntField(params, "volume", 0, 100);
    }
    if (action == "connect_bluetooth") {
        return requireStringField(params, "device");
    }
    if (action == "incoming_call") {
        return requireStringField(params, "caller");
    }
    if (action == "start_navigation_alert") {
        auto r = requireStringField(params, "message");
        if (!r.success) return r;
        return requireStringField(params, "priority");
    }
    if (action == "update_telemetry") {
        auto r = requireIntField(params, "speed", 0, 240);
        if (!r.success) return r;
        r = requireIntField(params, "fuel", 0, 100);
        if (!r.success) return r;
        return requireIntField(params, "temperature", -20, 140);
    }
    if (action == "change_setting") {
        auto r = requireStringField(params, "key");
        if (!r.success) return r;
        return requireStringField(params, "value");
    }
    if (action == "switch_screen") {
        return requireStringField(params, "screen");
    }
    if (action == "queue_notification") {
        return requireStringField(params, "message");
    }
    // Actions with no required params
    return CommandResult::ok();
}

CommandResult InputValidator::requireStringField(const nlohmann::json& params, const std::string& field) {
    if (!params.contains(field)) {
        return CommandResult::error("MISSING_FIELD", "Missing required field: " + field);
    }
    if (!params[field].is_string()) {
        return CommandResult::error("INVALID_FIELD_TYPE", "Field '" + field + "' must be a string");
    }
    if (params[field].get<std::string>().empty()) {
        return CommandResult::error("MISSING_FIELD", "Field '" + field + "' must not be empty");
    }
    return CommandResult::ok();
}

CommandResult InputValidator::requireIntField(const nlohmann::json& params, const std::string& field,
                                               int minVal, int maxVal) {
    if (!params.contains(field)) {
        return CommandResult::error("MISSING_FIELD", "Missing required field: " + field);
    }
    if (!params[field].is_number_integer()) {
        return CommandResult::error("INVALID_FIELD_TYPE", "Field '" + field + "' must be an integer");
    }
    int val = params[field].get<int>();
    if (val < minVal || val > maxVal) {
        return CommandResult::error("VALUE_OUT_OF_RANGE",
                                   "Field '" + field + "' must be between " +
                                   std::to_string(minVal) + " and " + std::to_string(maxVal));
    }
    return CommandResult::ok();
}

CommandResult InputValidator::requireField(const nlohmann::json& params, const std::string& field) {
    if (!params.contains(field)) {
        return CommandResult::error("MISSING_FIELD", "Missing required field: " + field);
    }
    return CommandResult::ok();
}

} // namespace infotainment
