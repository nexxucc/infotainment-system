#include "CommandServer.h"
#include "EventBus.h"
#include "StateManager.h"
#include "Logger.h"
#include "../modules/MediaManager.h"
#include "../modules/BluetoothManager.h"
#include "../modules/NavigationManager.h"
#include "../modules/TelemetryManager.h"
#include "../modules/SettingsManager.h"
#include "../modules/NotificationManager.h"
#include "../validation/InputValidator.h"

#include <QHostAddress>
#include <iostream>

namespace infotainment {

CommandServer::CommandServer(EventBus& eventBus,
                             StateManager& stateManager,
                             Logger& logger,
                             MediaManager& mediaManager,
                             BluetoothManager& bluetoothManager,
                             NavigationManager& navigationManager,
                             TelemetryManager& telemetryManager,
                             SettingsManager& settingsManager,
                             NotificationManager& notificationManager,
                             QObject* parent)
    : QObject(parent)
    , server_(new QTcpServer(this))
    , eventBus_(eventBus)
    , stateManager_(stateManager)
    , logger_(logger)
    , mediaManager_(mediaManager)
    , bluetoothManager_(bluetoothManager)
    , navigationManager_(navigationManager)
    , telemetryManager_(telemetryManager)
    , settingsManager_(settingsManager)
    , notificationManager_(notificationManager)
{
    QObject::connect(server_, &QTcpServer::newConnection, this, &CommandServer::onNewConnection);
}

bool CommandServer::start(const QString& address, quint16 port) {
    if (!server_->listen(QHostAddress(address), port)) {
        logger_.error("CommandServer", "ErrorRaised",
                     "Failed to start server on " + address.toStdString() + ":" + std::to_string(port));
        return false;
    }
    logger_.info("CommandServer", "ServerStarted",
                "Server listening on " + address.toStdString() + ":" + std::to_string(server_->serverPort()));
    return true;
}

void CommandServer::stop() {
    for (auto* client : clients_) {
        client->disconnectFromHost();
    }
    clients_.clear();
    buffers_.clear();
    server_->close();
}

quint16 CommandServer::serverPort() const {
    return server_->serverPort();
}

void CommandServer::onNewConnection() {
    while (server_->hasPendingConnections()) {
        QTcpSocket* client = server_->nextPendingConnection();
        clients_.append(client);
        buffers_[client] = QByteArray();
        QObject::connect(client, &QTcpSocket::readyRead, this, &CommandServer::onClientData);
        QObject::connect(client, &QTcpSocket::disconnected, this, &CommandServer::onClientDisconnected);
        logger_.info("CommandServer", "ClientConnected",
                    "Client connected: " + client->peerAddress().toString().toStdString());
    }
}

void CommandServer::onClientData() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    buffers_[client].append(client->readAll());

    // Process newline-delimited JSON
    while (true) {
        int idx = buffers_[client].indexOf('\n');
        if (idx < 0) break;

        QByteArray line = buffers_[client].left(idx).trimmed();
        buffers_[client].remove(0, idx + 1);

        if (line.isEmpty()) continue;

        nlohmann::json response;
        try {
            auto cmd = nlohmann::json::parse(line.toStdString());
            response = processCommand(cmd);
        } catch (const nlohmann::json::parse_error& e) {
            response = makeResponse("", "error", "INVALID_JSON",
                                   std::string("JSON parse error: ") + e.what());
        } catch (const std::exception& e) {
            response = makeResponse("", "error", "INTERNAL_ERROR",
                                   std::string("Internal error: ") + e.what());
        }

        std::string responseStr = response.dump() + "\n";
        client->write(QByteArray::fromStdString(responseStr));
        client->flush();
    }
}

void CommandServer::onClientDisconnected() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    clients_.removeAll(client);
    buffers_.remove(client);
    logger_.info("CommandServer", "ClientDisconnected", "Client disconnected");
    client->deleteLater();
}

nlohmann::json CommandServer::processCommand(const nlohmann::json& cmd) {
    // Validate structure
    auto validation = InputValidator::validateCommandStructure(cmd);
    if (!validation.success) {
        std::string id = cmd.contains("id") ? cmd["id"].get<std::string>() : "";
        return makeResponse(id, "error", validation.errorCode, validation.message);
    }

    std::string id = cmd["id"].is_string() ? cmd["id"].get<std::string>() : std::to_string(cmd["id"].get<int>());
    std::string action = cmd["action"].get<std::string>();
    nlohmann::json params = cmd["params"];

    // Validate params
    auto paramValidation = InputValidator::validateParams(action, params);
    if (!paramValidation.success) {
        return makeResponse(id, "error", paramValidation.errorCode, paramValidation.message);
    }

    CommandResult result = CommandResult::ok();

    try {
        if (action == "play_media") {
            result = mediaManager_.play(params["track"].get<std::string>(), id);
        } else if (action == "pause_media") {
            result = mediaManager_.pause(id);
        } else if (action == "stop_media") {
            result = mediaManager_.stop(id);
        } else if (action == "next_media") {
            result = mediaManager_.next(id);
        } else if (action == "previous_media") {
            result = mediaManager_.previous(id);
        } else if (action == "set_volume") {
            result = mediaManager_.setVolume(params["volume"].get<int>(), id);
        } else if (action == "connect_bluetooth") {
            result = bluetoothManager_.connect(params["device"].get<std::string>(), id);
        } else if (action == "disconnect_bluetooth") {
            result = bluetoothManager_.disconnect(id);
        } else if (action == "incoming_call") {
            result = bluetoothManager_.incomingCall(params["caller"].get<std::string>(), id);
        } else if (action == "accept_call") {
            result = bluetoothManager_.acceptCall(id);
        } else if (action == "reject_call") {
            result = bluetoothManager_.rejectCall(id);
        } else if (action == "end_call") {
            result = bluetoothManager_.endCall(id);
        } else if (action == "start_navigation_alert") {
            result = navigationManager_.startAlert(
                params["message"].get<std::string>(),
                params["priority"].get<std::string>(), id);
        } else if (action == "end_navigation_alert") {
            result = navigationManager_.endAlert(id);
        } else if (action == "update_telemetry") {
            result = telemetryManager_.updateAll(
                params["speed"].get<int>(),
                params["fuel"].get<int>(),
                params["temperature"].get<int>(), id);
        } else if (action == "change_setting") {
            result = settingsManager_.changeSetting(
                params["key"].get<std::string>(),
                params["value"].get<std::string>(), id);
        } else if (action == "switch_screen") {
            auto screen = activeScreenFromString(params["screen"].get<std::string>());
            stateManager_.setActiveScreen(screen);
            result = CommandResult::ok("Screen switched to " + params["screen"].get<std::string>());
        } else if (action == "queue_notification") {
            std::string priority = params.value("priority", "normal");
            result = notificationManager_.queueNotification(
                params["message"].get<std::string>(), priority, id);
        } else if (action == "display_notification") {
            result = notificationManager_.displayNext(id);
        } else if (action == "get_state") {
            auto resp = makeResponse(id, "ok", "", "Current state");
            resp["state"] = stateManager_.toJson();
            return resp;
        } else if (action == "get_recent_events") {
            auto resp = makeResponse(id, "ok", "", "Recent events");
            resp["events"] = eventBus_.getRecentEvents();
            return resp;
        } else if (action == "reset_state") {
            stateManager_.reset();
            logger_.info("CommandServer", "StateReset", "State reset to defaults");
            result = CommandResult::ok("State reset");
        }
    } catch (const std::exception& e) {
        return makeResponse(id, "error", "INTERNAL_ERROR",
                           std::string("Exception: ") + e.what());
    }

    auto resp = makeResponse(id,
                             result.success ? "ok" : "error",
                             result.errorCode,
                             result.message);
    resp["state"] = stateManager_.toJson();
    return resp;
}

nlohmann::json CommandServer::makeResponse(const std::string& id, const std::string& status,
                                            const std::string& errorCode, const std::string& message) {
    nlohmann::json resp;
    resp["id"] = id;
    resp["status"] = status;
    resp["error_code"] = errorCode.empty() ? nlohmann::json(nullptr) : nlohmann::json(errorCode);
    resp["message"] = message;
    return resp;
}

} // namespace infotainment
