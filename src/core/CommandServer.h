#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <memory>
#include <nlohmann/json.hpp>

namespace infotainment {

class EventBus;
class StateManager;
class Logger;
class MediaManager;
class BluetoothManager;
class NavigationManager;
class TelemetryManager;
class SettingsManager;
class NotificationManager;

/// TCP JSON command server using Qt networking (non-blocking, async).
class CommandServer : public QObject {
    Q_OBJECT

public:
    CommandServer(EventBus& eventBus,
                  StateManager& stateManager,
                  Logger& logger,
                  MediaManager& mediaManager,
                  BluetoothManager& bluetoothManager,
                  NavigationManager& navigationManager,
                  TelemetryManager& telemetryManager,
                  SettingsManager& settingsManager,
                  NotificationManager& notificationManager,
                  QObject* parent = nullptr);

    bool start(const QString& address = "127.0.0.1", quint16 port = 5555);
    void stop();
    quint16 serverPort() const;

private slots:
    void onNewConnection();
    void onClientData();
    void onClientDisconnected();

private:
    nlohmann::json processCommand(const nlohmann::json& cmd);
    nlohmann::json makeResponse(const std::string& id, const std::string& status,
                                 const std::string& errorCode, const std::string& message);

    QTcpServer* server_;
    QList<QTcpSocket*> clients_;
    QMap<QTcpSocket*, QByteArray> buffers_;

    EventBus& eventBus_;
    StateManager& stateManager_;
    Logger& logger_;
    MediaManager& mediaManager_;
    BluetoothManager& bluetoothManager_;
    NavigationManager& navigationManager_;
    TelemetryManager& telemetryManager_;
    SettingsManager& settingsManager_;
    NotificationManager& notificationManager_;
};

} // namespace infotainment
