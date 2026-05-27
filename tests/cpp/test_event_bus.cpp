#include <gtest/gtest.h>
#include "core/Logger.h"
#include "core/EventBus.h"
#include "core/Event.h"

#include <QCoreApplication>
#include <vector>
#include <stdexcept>

class EventBusTest : public ::testing::Test {
protected:
    void SetUp() override {
        logger_ = std::make_unique<infotainment::Logger>("logs/test_events.jsonl");
        bus_ = std::make_unique<infotainment::EventBus>(*logger_);
    }

    void TearDown() override {
        bus_->clearSubscriptions();
    }

    std::unique_ptr<infotainment::Logger> logger_;
    std::unique_ptr<infotainment::EventBus> bus_;
};

TEST_F(EventBusTest, SubscribeAndPublish) {
    bool called = false;
    bus_->subscribe(infotainment::EventType::MediaPlayRequested,
                    [&called](const infotainment::Event&) { called = true; });

    bus_->publish(infotainment::Event(infotainment::EventType::MediaPlayRequested));
    EXPECT_TRUE(called);
}

TEST_F(EventBusTest, MultipleSubscribers) {
    int callCount = 0;
    bus_->subscribe(infotainment::EventType::VolumeChanged,
                    [&callCount](const infotainment::Event&) { callCount++; });
    bus_->subscribe(infotainment::EventType::VolumeChanged,
                    [&callCount](const infotainment::Event&) { callCount++; });

    bus_->publish(infotainment::Event(infotainment::EventType::VolumeChanged, {{"volume", 50}}));
    EXPECT_EQ(callCount, 2);
}

TEST_F(EventBusTest, OnlySubscribedTypeReceived) {
    bool mediaCalled = false;
    bool btCalled = false;

    bus_->subscribe(infotainment::EventType::MediaPlayRequested,
                    [&mediaCalled](const infotainment::Event&) { mediaCalled = true; });
    bus_->subscribe(infotainment::EventType::BluetoothDeviceConnected,
                    [&btCalled](const infotainment::Event&) { btCalled = true; });

    bus_->publish(infotainment::Event(infotainment::EventType::MediaPlayRequested));
    EXPECT_TRUE(mediaCalled);
    EXPECT_FALSE(btCalled);
}

TEST_F(EventBusTest, ExceptionSafety) {
    int callCount = 0;

    // First handler throws
    bus_->subscribe(infotainment::EventType::MediaPlayRequested,
                    [](const infotainment::Event&) { throw std::runtime_error("test error"); });
    // Second handler should still be called
    bus_->subscribe(infotainment::EventType::MediaPlayRequested,
                    [&callCount](const infotainment::Event&) { callCount++; });

    EXPECT_NO_THROW(bus_->publish(infotainment::Event(infotainment::EventType::MediaPlayRequested)));
    EXPECT_EQ(callCount, 1);
}

TEST_F(EventBusTest, Unsubscribe) {
    int callCount = 0;
    int subId = bus_->subscribe(infotainment::EventType::MediaPlayRequested,
                                [&callCount](const infotainment::Event&) { callCount++; });

    bus_->publish(infotainment::Event(infotainment::EventType::MediaPlayRequested));
    EXPECT_EQ(callCount, 1);

    bus_->unsubscribe(subId);
    bus_->publish(infotainment::Event(infotainment::EventType::MediaPlayRequested));
    EXPECT_EQ(callCount, 1); // Not called again
}

TEST_F(EventBusTest, RecentEventsRingBuffer) {
    for (int i = 0; i < 110; i++) {
        bus_->publish(infotainment::Event(infotainment::EventType::VolumeChanged, {{"volume", i}}));
    }

    auto recent = bus_->getRecentEvents();
    EXPECT_EQ(recent.size(), 100); // Capped at 100
}

TEST_F(EventBusTest, RecentEventsContainData) {
    bus_->publish(infotainment::Event(infotainment::EventType::MediaPlayRequested, {{"track", "test.mp3"}}));

    auto recent = bus_->getRecentEvents();
    ASSERT_EQ(recent.size(), 1);
    EXPECT_EQ(recent[0]["type"], "MediaPlayRequested");
    EXPECT_EQ(recent[0]["data"]["track"], "test.mp3");
}

TEST_F(EventBusTest, EventDataPassedToHandler) {
    std::string receivedTrack;
    bus_->subscribe(infotainment::EventType::MediaPlayRequested,
                    [&receivedTrack](const infotainment::Event& e) {
                        receivedTrack = e.data.value("track", "");
                    });

    bus_->publish(infotainment::Event(infotainment::EventType::MediaPlayRequested, {{"track", "song.mp3"}}));
    EXPECT_EQ(receivedTrack, "song.mp3");
}

TEST_F(EventBusTest, ClearSubscriptions) {
    int callCount = 0;
    bus_->subscribe(infotainment::EventType::MediaPlayRequested,
                    [&callCount](const infotainment::Event&) { callCount++; });

    bus_->clearSubscriptions();
    bus_->publish(infotainment::Event(infotainment::EventType::MediaPlayRequested));
    EXPECT_EQ(callCount, 0);
}
