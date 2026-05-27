#pragma once

#include "../modules/MediaManager.h" // for CommandResult
#include <string>
#include <nlohmann/json.hpp>

namespace infotainment {

class InputValidator {
public:
    /// Validate that the JSON has required top-level fields: id, action, params
    static CommandResult validateCommandStructure(const nlohmann::json& cmd);

    /// Validate params for a specific action
    static CommandResult validateParams(const std::string& action, const nlohmann::json& params);

    /// Check if a string field exists and is non-empty
    static CommandResult requireStringField(const nlohmann::json& params, const std::string& field);

    /// Check if an integer field exists and is within range
    static CommandResult requireIntField(const nlohmann::json& params, const std::string& field,
                                          int minVal, int maxVal);

    /// Check if a field exists (any type)
    static CommandResult requireField(const nlohmann::json& params, const std::string& field);
};

} // namespace infotainment
