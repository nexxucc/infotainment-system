#include <gtest/gtest.h>
#include "validation/InputValidator.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using infotainment::InputValidator;

TEST(InputValidatorTest, ValidCommand) {
    json cmd = {{"id", "1"}, {"action", "play_media"}, {"params", {{"track", "song.mp3"}}}};
    auto result = InputValidator::validateCommandStructure(cmd);
    EXPECT_TRUE(result.success);
}

TEST(InputValidatorTest, MissingId) {
    json cmd = {{"action", "play_media"}, {"params", {}}};
    auto result = InputValidator::validateCommandStructure(cmd);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
}

TEST(InputValidatorTest, MissingAction) {
    json cmd = {{"id", "1"}, {"params", {}}};
    auto result = InputValidator::validateCommandStructure(cmd);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
}

TEST(InputValidatorTest, MissingParams) {
    json cmd = {{"id", "1"}, {"action", "play_media"}};
    auto result = InputValidator::validateCommandStructure(cmd);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
}

TEST(InputValidatorTest, ActionNotString) {
    json cmd = {{"id", "1"}, {"action", 123}, {"params", {}}};
    auto result = InputValidator::validateCommandStructure(cmd);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_FIELD_TYPE");
}

TEST(InputValidatorTest, ParamsNotObject) {
    json cmd = {{"id", "1"}, {"action", "play_media"}, {"params", "string"}};
    auto result = InputValidator::validateCommandStructure(cmd);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_FIELD_TYPE");
}

TEST(InputValidatorTest, UnknownAction) {
    json cmd = {{"id", "1"}, {"action", "fly_to_moon"}, {"params", json::object()}};
    auto result = InputValidator::validateCommandStructure(cmd);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "UNKNOWN_ACTION");
}

TEST(InputValidatorTest, PlayMediaMissingTrack) {
    json params = {};
    auto result = InputValidator::validateParams("play_media", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
}

TEST(InputValidatorTest, PlayMediaTrackNotString) {
    json params = {{"track", 123}};
    auto result = InputValidator::validateParams("play_media", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_FIELD_TYPE");
}

TEST(InputValidatorTest, SetVolumeValidRange) {
    json params = {{"volume", 50}};
    auto result = InputValidator::validateParams("set_volume", params);
    EXPECT_TRUE(result.success);
}

TEST(InputValidatorTest, SetVolumeBelowZero) {
    json params = {{"volume", -1}};
    auto result = InputValidator::validateParams("set_volume", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "VALUE_OUT_OF_RANGE");
}

TEST(InputValidatorTest, SetVolumeAbove100) {
    json params = {{"volume", 101}};
    auto result = InputValidator::validateParams("set_volume", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "VALUE_OUT_OF_RANGE");
}

TEST(InputValidatorTest, UpdateTelemetryValid) {
    json params = {{"speed", 60}, {"fuel", 72}, {"temperature", 90}};
    auto result = InputValidator::validateParams("update_telemetry", params);
    EXPECT_TRUE(result.success);
}

TEST(InputValidatorTest, UpdateTelemetrySpeedTooHigh) {
    json params = {{"speed", 241}, {"fuel", 72}, {"temperature", 90}};
    auto result = InputValidator::validateParams("update_telemetry", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "VALUE_OUT_OF_RANGE");
}

TEST(InputValidatorTest, UpdateTelemetryFuelOutOfRange) {
    json params = {{"speed", 60}, {"fuel", 101}, {"temperature", 90}};
    auto result = InputValidator::validateParams("update_telemetry", params);
    EXPECT_FALSE(result.success);
}

TEST(InputValidatorTest, UpdateTelemetryTempOutOfRange) {
    json params = {{"speed", 60}, {"fuel", 72}, {"temperature", 141}};
    auto result = InputValidator::validateParams("update_telemetry", params);
    EXPECT_FALSE(result.success);
}

TEST(InputValidatorTest, ConnectBluetoothMissingDevice) {
    json params = {};
    auto result = InputValidator::validateParams("connect_bluetooth", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
}

TEST(InputValidatorTest, IncomingCallMissingCaller) {
    json params = {};
    auto result = InputValidator::validateParams("incoming_call", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
}

TEST(InputValidatorTest, NavigationAlertMissingMessage) {
    json params = {{"priority", "high"}};
    auto result = InputValidator::validateParams("start_navigation_alert", params);
    EXPECT_FALSE(result.success);
}

TEST(InputValidatorTest, NavigationAlertMissingPriority) {
    json params = {{"message", "Turn left"}};
    auto result = InputValidator::validateParams("start_navigation_alert", params);
    EXPECT_FALSE(result.success);
}

TEST(InputValidatorTest, NoParamsActions) {
    for (const auto& action : {"pause_media", "stop_media", "get_state", "reset_state"}) {
        json params = {};
        auto result = InputValidator::validateParams(action, params);
        EXPECT_TRUE(result.success) << "Failed for action: " << action;
    }
}

TEST(InputValidatorTest, MalformedJsonInput) {
    json cmd = json::array();  // Array instead of object
    auto result = InputValidator::validateCommandStructure(cmd);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_JSON");
}

TEST(InputValidatorTest, EmptyStringField) {
    json params = {{"track", ""}};
    auto result = InputValidator::validateParams("play_media", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
}

TEST(InputValidatorTest, VolumeNotInteger) {
    json params = {{"volume", "fifty"}};
    auto result = InputValidator::validateParams("set_volume", params);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_FIELD_TYPE");
}
