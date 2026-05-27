#pragma once

#include "MediaManager.h" // for CommandResult
#include <string>
#include <queue>
#include <mutex>

namespace infotainment {

class EventBus;
class StateManager;
class Logger;

struct Notification {
    std::string message;
    std::string priority; // "low", "normal", "high"
    std::string timestamp;
};

class NotificationManager {
public:
    NotificationManager(EventBus& eventBus, StateManager& stateManager, Logger& logger);

    CommandResult queueNotification(const std::string& message, const std::string& priority = "normal",
                                     const std::string& correlationId = "");
    CommandResult displayNext(const std::string& correlationId = "");
    int getPendingCount() const;
    std::vector<Notification> getPendingNotifications() const;

private:
    EventBus& eventBus_;
    StateManager& stateManager_;
    Logger& logger_;
    mutable std::mutex mutex_;
    std::queue<Notification> pendingQueue_;
};

} // namespace infotainment
