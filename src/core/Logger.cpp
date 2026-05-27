#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace infotainment {

Logger::Logger(const std::string& filePath) {
    // Ensure directory exists
    std::filesystem::path p(filePath);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }
    file_.open(filePath, std::ios::app);
    if (!file_.is_open()) {
        std::cerr << "Logger: Failed to open log file: " << filePath << std::endl;
    }
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_.is_open()) {
        file_.close();
    }
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm{};
    gmtime_r(&time_t, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    return oss.str();
}

void Logger::log(LogLevel level,
                 const std::string& component,
                 const std::string& eventType,
                 const std::string& message,
                 const nlohmann::json& beforeState,
                 const nlohmann::json& afterState,
                 const std::string& correlationId) {
    nlohmann::json entry;
    entry["timestamp"] = getTimestamp();
    entry["level"] = logLevelToString(level);
    entry["component"] = component;
    entry["event_type"] = eventType;
    entry["message"] = message;
    entry["before_state"] = beforeState;
    entry["after_state"] = afterState;
    entry["correlation_id"] = correlationId;

    std::lock_guard<std::mutex> lock(mutex_);

    // Write to file
    if (file_.is_open()) {
        file_ << entry.dump() << '\n';
        file_.flush();
    }

    // Store in memory
    recentLogs_.push_back(entry);
    while (recentLogs_.size() > kMaxRecentLogs) {
        recentLogs_.pop_front();
    }

    // Notify callback
    if (logCallback_) {
        logCallback_(entry);
    }
}

void Logger::info(const std::string& component, const std::string& eventType,
                  const std::string& message,
                  const nlohmann::json& beforeState,
                  const nlohmann::json& afterState,
                  const std::string& correlationId) {
    log(LogLevel::INFO, component, eventType, message, beforeState, afterState, correlationId);
}

void Logger::warn(const std::string& component, const std::string& eventType,
                  const std::string& message,
                  const nlohmann::json& beforeState,
                  const nlohmann::json& afterState,
                  const std::string& correlationId) {
    log(LogLevel::WARN, component, eventType, message, beforeState, afterState, correlationId);
}

void Logger::error(const std::string& component, const std::string& eventType,
                   const std::string& message,
                   const nlohmann::json& beforeState,
                   const nlohmann::json& afterState,
                   const std::string& correlationId) {
    log(LogLevel::ERROR, component, eventType, message, beforeState, afterState, correlationId);
}

void Logger::setLogCallback(LogCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    logCallback_ = std::move(callback);
}

std::vector<nlohmann::json> Logger::getRecentLogs(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t n = std::min(count, recentLogs_.size());
    return std::vector<nlohmann::json>(recentLogs_.end() - n, recentLogs_.end());
}

} // namespace infotainment
