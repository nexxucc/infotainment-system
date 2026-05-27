#include "MediaManager.h"
#include "../core/EventBus.h"
#include "../core/StateManager.h"
#include "../core/Logger.h"

namespace infotainment {

MediaManager::MediaManager(EventBus& eventBus, StateManager& stateManager, Logger& logger)
    : eventBus_(eventBus), stateManager_(stateManager), logger_(logger) {}

CommandResult MediaManager::play(const std::string& track, const std::string& correlationId) {
    if (track.empty()) {
        return CommandResult::error("MISSING_FIELD", "Track name is required");
    }

    auto beforeState = stateManager_.toJson();
    auto currentState = stateManager_.getMediaState();

    // Can play from Stopped or Paused
    if (currentState == MediaState::Playing) {
        // Already playing, update track
    }

    stateManager_.setTrack(track);
    stateManager_.setMediaState(MediaState::Playing);

    auto afterState = stateManager_.toJson();
    logger_.info("MediaManager", "MediaPlayRequested",
                 "Media started: " + track, beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::MediaPlayRequested, {{"track", track}}, correlationId));

    return CommandResult::ok("Media playing: " + track);
}

CommandResult MediaManager::pause(const std::string& correlationId) {
    auto currentState = stateManager_.getMediaState();
    if (currentState != MediaState::Playing) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "Cannot pause: media is not playing");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setMediaState(MediaState::Paused);
    auto afterState = stateManager_.toJson();

    logger_.info("MediaManager", "MediaPauseRequested",
                 "Media paused", beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::MediaPauseRequested, {}, correlationId));

    return CommandResult::ok("Media paused");
}

CommandResult MediaManager::stop(const std::string& correlationId) {
    auto currentState = stateManager_.getMediaState();
    if (currentState == MediaState::Stopped) {
        return CommandResult::error("INVALID_STATE_TRANSITION",
                                   "Cannot stop: media is already stopped");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setMediaState(MediaState::Stopped);
    stateManager_.setTrack("");
    auto afterState = stateManager_.toJson();

    logger_.info("MediaManager", "MediaStopRequested",
                 "Media stopped", beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::MediaStopRequested, {}, correlationId));

    return CommandResult::ok("Media stopped");
}

CommandResult MediaManager::next(const std::string& correlationId) {
    auto beforeState = stateManager_.toJson();

    logger_.info("MediaManager", "MediaNextRequested",
                 "Next track requested", beforeState, beforeState, correlationId);
    eventBus_.publish(Event(EventType::MediaNextRequested, {}, correlationId));

    return CommandResult::ok("Next track");
}

CommandResult MediaManager::previous(const std::string& correlationId) {
    auto beforeState = stateManager_.toJson();

    logger_.info("MediaManager", "MediaPreviousRequested",
                 "Previous track requested", beforeState, beforeState, correlationId);
    eventBus_.publish(Event(EventType::MediaPreviousRequested, {}, correlationId));

    return CommandResult::ok("Previous track");
}

CommandResult MediaManager::setVolume(int volume, const std::string& correlationId) {
    if (volume < 0 || volume > 100) {
        return CommandResult::error("VALUE_OUT_OF_RANGE",
                                   "Volume must be between 0 and 100");
    }

    auto beforeState = stateManager_.toJson();
    stateManager_.setVolume(volume);
    auto afterState = stateManager_.toJson();

    logger_.info("MediaManager", "VolumeChanged",
                 "Volume set to " + std::to_string(volume),
                 beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::VolumeChanged, {{"volume", volume}}, correlationId));

    return CommandResult::ok("Volume set to " + std::to_string(volume));
}

} // namespace infotainment
