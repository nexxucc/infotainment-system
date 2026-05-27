#include "NotificationManager.h"
#include "../core/EventBus.h"
#include "../core/StateManager.h"
#include "../core/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace infotainment {

NotificationManager::NotificationManager(EventBus& eventBus, StateManager& stateManager, Logger& logger)
    : eventBus_(eventBus), stateManager_(stateManager), logger_(logger) {}

CommandResult NotificationManager::queueNotification(const std::string& message, const std::string& priority,
                                                      const std::string& correlationId) {
    if (message.empty()) {
        return CommandResult::error("MISSING_FIELD", "Notification message is required");
    }

    auto beforeState = stateManager_.toJson();

    // Generate timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&time_t, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");

    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingQueue_.push({message, priority, oss.str()});
    }

    stateManager_.setNotificationsPending(getPendingCount());

    auto afterState = stateManager_.toJson();
    logger_.info("NotificationManager", "NotificationQueued",
                 "Notification queued: " + message,
                 beforeState, afterState, correlationId);
    eventBus_.publish(Event(EventType::NotificationQueued,
                           {{"message", message}, {"priority", priority}},
                           correlationId));

    // Low-priority notifications during active calls stay queued (spec requirement)
    auto callState = stateManager_.getCallState();
    if (callState != CallState::Idle && priority == "low") {
        return CommandResult::ok("Notification queued (deferred during call)");
    }

    return CommandResult::ok("Notification queued");
}

CommandResult NotificationManager::displayNext(const std::string& correlationId) {
    Notification notif;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pendingQueue_.empty()) {
            return CommandResult::error("INVALID_STATE_TRANSITION", "No pending notifications");
        }
        notif = pendingQueue_.front();
        pendingQueue_.pop();
    }

    stateManager_.setNotificationsPending(getPendingCount());

    auto state = stateManager_.toJson();
    logger_.info("NotificationManager", "NotificationDisplayed",
                 "Displaying: " + notif.message, state, state, correlationId);
    eventBus_.publish(Event(EventType::NotificationDisplayed,
                           {{"message", notif.message}}, correlationId));

    return CommandResult::ok("Displaying notification: " + notif.message);
}

int NotificationManager::getPendingCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int>(pendingQueue_.size());
}

std::vector<Notification> NotificationManager::getPendingNotifications() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Notification> result;
    auto copy = pendingQueue_;
    while (!copy.empty()) {
        result.push_back(copy.front());
        copy.pop();
    }
    return result;
}

} // namespace infotainment
