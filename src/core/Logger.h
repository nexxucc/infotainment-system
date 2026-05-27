#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <deque>
#include <functional>
#include <nlohmann/json.hpp>

namespace infotainment {

enum class LogLevel { INFO, WARN, ERROR };

inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

class Logger {
public:
    using LogCallback = std::function<void(const nlohmann::json&)>;

    explicit Logger(const std::string& filePath = "logs/infotainment_events.jsonl");
    ~Logger();

    void log(LogLevel level,
             const std::string& component,
             const std::string& eventType,
             const std::string& message,
             const nlohmann::json& beforeState = {},
             const nlohmann::json& afterState = {},
             const std::string& correlationId = "");

    void info(const std::string& component, const std::string& eventType,
              const std::string& message,
              const nlohmann::json& beforeState = {},
              const nlohmann::json& afterState = {},
              const std::string& correlationId = "");

    void warn(const std::string& component, const std::string& eventType,
              const std::string& message,
              const nlohmann::json& beforeState = {},
              const nlohmann::json& afterState = {},
              const std::string& correlationId = "");

    void error(const std::string& component, const std::string& eventType,
               const std::string& message,
               const nlohmann::json& beforeState = {},
               const nlohmann::json& afterState = {},
               const std::string& correlationId = "");

    /// Set a callback that receives each log entry (for UI activity log)
    void setLogCallback(LogCallback callback);

    /// Get recent log entries from in-memory buffer
    std::vector<nlohmann::json> getRecentLogs(size_t count = 50) const;

private:
    std::string getTimestamp() const;

    mutable std::mutex mutex_;
    std::ofstream file_;
    std::deque<nlohmann::json> recentLogs_;
    static constexpr size_t kMaxRecentLogs = 200;
    LogCallback logCallback_;
};

} // namespace infotainment
