#pragma once

#include "Event.h"
#include <functional>
#include <vector>
#include <deque>
#include <mutex>
#include <unordered_map>

namespace infotainment {

class Logger;

class EventBus {
public:
    using EventHandler = std::function<void(const Event&)>;

    explicit EventBus(Logger& logger);

    /// Subscribe a handler for a specific event type. Returns subscription ID.
    int subscribe(EventType type, EventHandler handler);

    /// Unsubscribe by subscription ID.
    void unsubscribe(int subscriptionId);

    /// Publish an event to all subscribers. Exception-safe.
    void publish(const Event& event);

    /// Get recent events as JSON array.
    nlohmann::json getRecentEvents() const;

    /// Clear all subscriptions (useful for testing).
    void clearSubscriptions();

private:
    struct Subscription {
        int id;
        EventHandler handler;
    };

    Logger& logger_;
    mutable std::mutex mutex_;
    std::unordered_map<EventType, std::vector<Subscription>> subscribers_;
    std::deque<Event> recentEvents_;
    int nextSubscriptionId_ = 0;
    static constexpr size_t kMaxRecentEvents = 100;
};

} // namespace infotainment
