#include <QApplication>
#include "ui/MainWindow.h"
#include "../src/core/Logger.h"
#include "../src/core/EventBus.h"
#include "../src/core/StateManager.h"
#include "../src/core/CommandServer.h"
#include "../src/modules/MediaManager.h"
#include "../src/modules/BluetoothManager.h"
#include "../src/modules/NavigationManager.h"
#include "../src/modules/TelemetryManager.h"
#include "../src/modules/SettingsManager.h"
#include "../src/modules/NotificationManager.h"

#include <iostream>
#include <csignal>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Infotainment Simulator");
    app.setApplicationVersion("1.0.0");

    // Core infrastructure
    infotainment::Logger logger("logs/infotainment_events.jsonl");
    infotainment::EventBus eventBus(logger);
    infotainment::StateManager stateManager;

    // Managers (dependency injection via constructors)
    infotainment::MediaManager mediaManager(eventBus, stateManager, logger);
    infotainment::BluetoothManager bluetoothManager(eventBus, stateManager, logger);
    infotainment::NavigationManager navigationManager(eventBus, stateManager, logger);
    infotainment::TelemetryManager telemetryManager(eventBus, stateManager, logger);
    infotainment::SettingsManager settingsManager(eventBus, stateManager, logger);
    infotainment::NotificationManager notificationManager(eventBus, stateManager, logger);

    // TCP Command Server
    infotainment::CommandServer commandServer(
        eventBus, stateManager, logger,
        mediaManager, bluetoothManager, navigationManager,
        telemetryManager, settingsManager, notificationManager);

    quint16 port = 5555;
    // Allow port override via command line
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--port" && i + 1 < argc) {
            port = static_cast<quint16>(std::stoi(argv[i + 1]));
        }
    }

    if (!commandServer.start("127.0.0.1", port)) {
        std::cerr << "Failed to start command server on port " << port << std::endl;
        return 1;
    }
    std::cout << "Command server listening on 127.0.0.1:" << port << std::endl;

    // Main window
    infotainment::MainWindow mainWindow(stateManager,
        mediaManager, bluetoothManager, navigationManager,
        telemetryManager, settingsManager);

    // Wire logger callback to UI activity log
    logger.setLogCallback([&mainWindow](const nlohmann::json& entry) {
        QString logLine = QString("[%1] %2: %3 - %4")
            .arg(QString::fromStdString(entry.value("level", "")))
            .arg(QString::fromStdString(entry.value("component", "")))
            .arg(QString::fromStdString(entry.value("event_type", "")))
            .arg(QString::fromStdString(entry.value("message", "")));
        // Use invokeMethod for thread safety
        QMetaObject::invokeMethod(&mainWindow, [&mainWindow, logLine]() {
            mainWindow.appendLog(logLine);
        }, Qt::QueuedConnection);
    });

    mainWindow.show();

    logger.info("Application", "Startup", "Infotainment Simulator started on port " + std::to_string(port));

    return app.exec();
}
