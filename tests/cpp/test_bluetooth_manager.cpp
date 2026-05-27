#include <gtest/gtest.h>
#include "core/Logger.h"
#include "core/EventBus.h"
#include "core/StateManager.h"
#include "modules/BluetoothManager.h"

#include <QCoreApplication>

class BluetoothManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!QCoreApplication::instance()) {
            int argc = 0;
            app_ = std::make_unique<QCoreApplication>(argc, nullptr);
        }
        logger_ = std::make_unique<infotainment::Logger>("logs/test_events.jsonl");
        eventBus_ = std::make_unique<infotainment::EventBus>(*logger_);
        stateManager_ = std::make_unique<infotainment::StateManager>();
        bt_ = std::make_unique<infotainment::BluetoothManager>(*eventBus_, *stateManager_, *logger_);
    }

    std::unique_ptr<QCoreApplication> app_;
    std::unique_ptr<infotainment::Logger> logger_;
    std::unique_ptr<infotainment::EventBus> eventBus_;
    std::unique_ptr<infotainment::StateManager> stateManager_;
    std::unique_ptr<infotainment::BluetoothManager> bt_;
};

TEST_F(BluetoothManagerTest, ConnectDevice) {
    auto result = bt_->connect("Pixel 7");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getBluetoothState(), infotainment::BluetoothState::Connected);
    EXPECT_EQ(stateManager_->getConnectedDevice(), "Pixel 7");
}

TEST_F(BluetoothManagerTest, ConnectEmptyDevice) {
    auto result = bt_->connect("");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
}

TEST_F(BluetoothManagerTest, ConnectWhileConnected) {
    bt_->connect("Pixel 7");
    auto result = bt_->connect("iPhone 15");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_STATE_TRANSITION");
}

TEST_F(BluetoothManagerTest, Disconnect) {
    bt_->connect("Pixel 7");
    auto result = bt_->disconnect();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getBluetoothState(), infotainment::BluetoothState::Disconnected);
    EXPECT_TRUE(stateManager_->getConnectedDevice().empty());
}

TEST_F(BluetoothManagerTest, DisconnectWhileDisconnected) {
    auto result = bt_->disconnect();
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_STATE_TRANSITION");
}

TEST_F(BluetoothManagerTest, IncomingCallRequiresBluetooth) {
    auto result = bt_->incomingCall("+919999999999");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_STATE_TRANSITION");
}

TEST_F(BluetoothManagerTest, IncomingCallSuccess) {
    bt_->connect("Pixel 7");
    auto result = bt_->incomingCall("+919999999999");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getCallState(), infotainment::CallState::Ringing);
    EXPECT_EQ(stateManager_->getActiveScreen(), infotainment::ActiveScreen::CallOverlay);
}

TEST_F(BluetoothManagerTest, IncomingCallPausesMedia) {
    bt_->connect("Pixel 7");

    // Set media to playing
    stateManager_->setMediaState(infotainment::MediaState::Playing);
    stateManager_->setTrack("song.mp3");

    bt_->incomingCall("+919999999999");

    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Paused);
    EXPECT_EQ(stateManager_->getCallState(), infotainment::CallState::Ringing);
}

TEST_F(BluetoothManagerTest, AcceptCall) {
    bt_->connect("Pixel 7");
    bt_->incomingCall("+919999999999");
    auto result = bt_->acceptCall();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getCallState(), infotainment::CallState::Active);
}

TEST_F(BluetoothManagerTest, AcceptCallWithoutRinging) {
    auto result = bt_->acceptCall();
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_STATE_TRANSITION");
}

TEST_F(BluetoothManagerTest, RejectCall) {
    bt_->connect("Pixel 7");
    bt_->incomingCall("+919999999999");
    auto result = bt_->rejectCall();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getCallState(), infotainment::CallState::Idle);
    EXPECT_EQ(stateManager_->getActiveScreen(), infotainment::ActiveScreen::Dashboard);
}

TEST_F(BluetoothManagerTest, EndActiveCall) {
    bt_->connect("Pixel 7");
    bt_->incomingCall("+919999999999");
    bt_->acceptCall();
    auto result = bt_->endCall();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getCallState(), infotainment::CallState::Idle);
}

TEST_F(BluetoothManagerTest, EndCallWhenIdle) {
    auto result = bt_->endCall();
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_STATE_TRANSITION");
}

TEST_F(BluetoothManagerTest, DisconnectDuringCall) {
    bt_->connect("Pixel 7");
    bt_->incomingCall("+919999999999");
    bt_->acceptCall();

    auto result = bt_->disconnect();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getCallState(), infotainment::CallState::Idle);
    EXPECT_EQ(stateManager_->getBluetoothState(), infotainment::BluetoothState::Disconnected);
}

TEST_F(BluetoothManagerTest, DuplicateIncomingCall) {
    bt_->connect("Pixel 7");
    bt_->incomingCall("+919999999999");
    auto result = bt_->incomingCall("+918888888888");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_STATE_TRANSITION");
}
