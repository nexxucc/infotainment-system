#include <gtest/gtest.h>
#include "core/StateManager.h"

#include <QCoreApplication>

class StateManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // QCoreApplication needed for signals
        if (!QCoreApplication::instance()) {
            int argc = 0;
            app_ = std::make_unique<QCoreApplication>(argc, nullptr);
        }
        sm_ = std::make_unique<infotainment::StateManager>();
    }

    std::unique_ptr<QCoreApplication> app_;
    std::unique_ptr<infotainment::StateManager> sm_;
};

TEST_F(StateManagerTest, DefaultState) {
    auto state = sm_->getState();
    EXPECT_EQ(state.mediaState, infotainment::MediaState::Stopped);
    EXPECT_EQ(state.volume, 50);
    EXPECT_TRUE(state.track.empty());
    EXPECT_EQ(state.bluetoothState, infotainment::BluetoothState::Disconnected);
    EXPECT_EQ(state.callState, infotainment::CallState::Idle);
    EXPECT_EQ(state.navigationState, infotainment::NavigationState::Idle);
    EXPECT_EQ(state.activeScreen, infotainment::ActiveScreen::Dashboard);
    EXPECT_EQ(state.vehicleSpeed, 0);
    EXPECT_EQ(state.fuelLevel, 100);
    EXPECT_EQ(state.engineTemperature, 20);
    EXPECT_EQ(state.notificationsPending, 0);
    EXPECT_TRUE(state.lastError.empty());
}

TEST_F(StateManagerTest, SetMediaState) {
    EXPECT_TRUE(sm_->setMediaState(infotainment::MediaState::Playing));
    EXPECT_EQ(sm_->getMediaState(), infotainment::MediaState::Playing);
}

TEST_F(StateManagerTest, VolumeValidRange) {
    EXPECT_TRUE(sm_->setVolume(0));
    EXPECT_EQ(sm_->getVolume(), 0);
    EXPECT_TRUE(sm_->setVolume(100));
    EXPECT_EQ(sm_->getVolume(), 100);
    EXPECT_TRUE(sm_->setVolume(50));
    EXPECT_EQ(sm_->getVolume(), 50);
}

TEST_F(StateManagerTest, VolumeOutOfRange) {
    EXPECT_FALSE(sm_->setVolume(-1));
    EXPECT_FALSE(sm_->setVolume(101));
    EXPECT_EQ(sm_->getVolume(), 50); // unchanged
}

TEST_F(StateManagerTest, SpeedValidRange) {
    EXPECT_TRUE(sm_->setVehicleSpeed(0));
    EXPECT_TRUE(sm_->setVehicleSpeed(240));
    EXPECT_EQ(sm_->getVehicleSpeed(), 240);
}

TEST_F(StateManagerTest, SpeedOutOfRange) {
    EXPECT_FALSE(sm_->setVehicleSpeed(-1));
    EXPECT_FALSE(sm_->setVehicleSpeed(241));
}

TEST_F(StateManagerTest, FuelValidRange) {
    EXPECT_TRUE(sm_->setFuelLevel(0));
    EXPECT_TRUE(sm_->setFuelLevel(100));
}

TEST_F(StateManagerTest, FuelOutOfRange) {
    EXPECT_FALSE(sm_->setFuelLevel(-1));
    EXPECT_FALSE(sm_->setFuelLevel(101));
}

TEST_F(StateManagerTest, TemperatureValidRange) {
    EXPECT_TRUE(sm_->setEngineTemperature(-20));
    EXPECT_TRUE(sm_->setEngineTemperature(140));
}

TEST_F(StateManagerTest, TemperatureOutOfRange) {
    EXPECT_FALSE(sm_->setEngineTemperature(-21));
    EXPECT_FALSE(sm_->setEngineTemperature(141));
}

TEST_F(StateManagerTest, Reset) {
    sm_->setMediaState(infotainment::MediaState::Playing);
    sm_->setTrack("test.mp3");
    sm_->setVolume(80);
    sm_->setVehicleSpeed(120);

    sm_->reset();

    auto state = sm_->getState();
    EXPECT_EQ(state.mediaState, infotainment::MediaState::Stopped);
    EXPECT_TRUE(state.track.empty());
    EXPECT_EQ(state.volume, 50);
    EXPECT_EQ(state.vehicleSpeed, 0);
}

TEST_F(StateManagerTest, JsonSerialization) {
    sm_->setMediaState(infotainment::MediaState::Playing);
    sm_->setTrack("song.mp3");
    sm_->setVolume(75);

    auto json = sm_->toJson();
    EXPECT_EQ(json["media_state"], "playing");
    EXPECT_EQ(json["track"], "song.mp3");
    EXPECT_EQ(json["volume"], 75);
    EXPECT_EQ(json["bluetooth_state"], "disconnected");
    EXPECT_EQ(json["call_state"], "idle");
    EXPECT_TRUE(json["connected_device"].is_null());
}

TEST_F(StateManagerTest, NotificationsPendingValidation) {
    EXPECT_TRUE(sm_->setNotificationsPending(5));
    EXPECT_EQ(sm_->getNotificationsPending(), 5);
    EXPECT_FALSE(sm_->setNotificationsPending(-1));
}

TEST_F(StateManagerTest, StateSignalEmitted) {
    bool signalReceived = false;
    QObject::connect(sm_.get(), &infotainment::StateManager::stateChanged,
                     [&signalReceived]() { signalReceived = true; });

    sm_->setMediaState(infotainment::MediaState::Playing);
    EXPECT_TRUE(signalReceived);
}
