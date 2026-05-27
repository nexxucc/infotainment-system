#include <gtest/gtest.h>
#include "core/Logger.h"
#include "core/EventBus.h"
#include "core/StateManager.h"
#include "modules/MediaManager.h"

#include <QCoreApplication>

class MediaManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!QCoreApplication::instance()) {
            int argc = 0;
            app_ = std::make_unique<QCoreApplication>(argc, nullptr);
        }
        logger_ = std::make_unique<infotainment::Logger>("logs/test_events.jsonl");
        eventBus_ = std::make_unique<infotainment::EventBus>(*logger_);
        stateManager_ = std::make_unique<infotainment::StateManager>();
        media_ = std::make_unique<infotainment::MediaManager>(*eventBus_, *stateManager_, *logger_);
    }

    std::unique_ptr<QCoreApplication> app_;
    std::unique_ptr<infotainment::Logger> logger_;
    std::unique_ptr<infotainment::EventBus> eventBus_;
    std::unique_ptr<infotainment::StateManager> stateManager_;
    std::unique_ptr<infotainment::MediaManager> media_;
};

TEST_F(MediaManagerTest, PlayFromStopped) {
    auto result = media_->play("song.mp3");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Playing);
    EXPECT_EQ(stateManager_->getTrack(), "song.mp3");
}

TEST_F(MediaManagerTest, PlayEmptyTrack) {
    auto result = media_->play("");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "MISSING_FIELD");
    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Stopped);
}

TEST_F(MediaManagerTest, PauseFromPlaying) {
    media_->play("song.mp3");
    auto result = media_->pause();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Paused);
}

TEST_F(MediaManagerTest, PauseFromStopped) {
    auto result = media_->pause();
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_STATE_TRANSITION");
}

TEST_F(MediaManagerTest, StopFromPlaying) {
    media_->play("song.mp3");
    auto result = media_->stop();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Stopped);
    EXPECT_TRUE(stateManager_->getTrack().empty());
}

TEST_F(MediaManagerTest, StopFromStopped) {
    auto result = media_->stop();
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "INVALID_STATE_TRANSITION");
}

TEST_F(MediaManagerTest, PlayPausePlay) {
    media_->play("track1.mp3");
    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Playing);

    media_->pause();
    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Paused);

    media_->play("track1.mp3");
    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Playing);
}

TEST_F(MediaManagerTest, SetVolumeValid) {
    auto result = media_->setVolume(75);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getVolume(), 75);
}

TEST_F(MediaManagerTest, SetVolumeBoundary) {
    EXPECT_TRUE(media_->setVolume(0).success);
    EXPECT_EQ(stateManager_->getVolume(), 0);

    EXPECT_TRUE(media_->setVolume(100).success);
    EXPECT_EQ(stateManager_->getVolume(), 100);
}

TEST_F(MediaManagerTest, SetVolumeOutOfRange) {
    auto result = media_->setVolume(-1);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "VALUE_OUT_OF_RANGE");

    result = media_->setVolume(101);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, "VALUE_OUT_OF_RANGE");
}

TEST_F(MediaManagerTest, NextAndPrevious) {
    auto result = media_->next();
    EXPECT_TRUE(result.success);

    result = media_->previous();
    EXPECT_TRUE(result.success);
}

TEST_F(MediaManagerTest, StopFromPaused) {
    media_->play("song.mp3");
    media_->pause();
    auto result = media_->stop();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(stateManager_->getMediaState(), infotainment::MediaState::Stopped);
}
