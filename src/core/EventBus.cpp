#include "EventBus.h"
#include "Logger.h"
#include <iostream>

namespace infotainment {

EventBus::EventBus(Logger& logger) : logger_(logger) {}

int EventBus::subscribe(EventType type, EventHandler handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    int id = nextSubscriptionId_++;
    subscribers_[type].push_back({id, std::move(handler)});
    return id;
}

void EventBus::unsubscribe(int subscriptionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [type, subs] : subscribers_) {
        std::erase_if(subs, [subscriptionId](const Subscription& s) {
            return s.id == subscriptionId;
        });
    }
}

void EventBus::publish(const Event& event) {
    std::vector<Subscription> handlers;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        // Store in ring buffer
        recentEvents_.push_back(event);
        while (recentEvents_.size() > kMaxRecentEvents) {
            recentEvents_.pop_front();
        }

        // Copy handlers to dispatch outside lock
        auto it = subscribers_.find(event.type);
        if (it != subscribers_.end()) {
            handlers = it->second;
        }
    }

    // Dispatch to all handlers, catching exceptions
    for (const auto& sub : handlers) {
        try {
            sub.handler(event);
        } catch (const std::exception& e) {
            logger_.error("EventBus", eventTypeToString(event.type),
                         std::string("Handler exception: ") + e.what());
        } catch (...) {
            logger_.error("EventBus", eventTypeToString(event.type),
                         "Handler threw unknown exception");
        }
    }
}

nlohmann::json EventBus::getRecentEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& event : recentEvents_) {
        arr.push_back(event.toJson());
    }
    return arr;
}

void EventBus::clearSubscriptions() {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_.clear();
    recentEvents_.clear();
}

} // namespace infotainment
