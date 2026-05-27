#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QDockWidget>

namespace infotainment {

class StateManager;
class MediaManager;
class BluetoothManager;
class NavigationManager;
class TelemetryManager;
class SettingsManager;
class NotificationManager;

class DashboardWidget;
class MediaWidget;
class BluetoothWidget;
class NavigationWidget;
class TelemetryWidget;
class SettingsWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(StateManager& stateManager,
                        MediaManager& mediaManager,
                        BluetoothManager& bluetoothManager,
                        NavigationManager& navigationManager,
                        TelemetryManager& telemetryManager,
                        SettingsManager& settingsManager,
                        QWidget* parent = nullptr);

    void appendLog(const QString& entry);
    void showError(const QString& error);

public slots:
    void onStateChanged();

private:
    void setupUI();
    void setupMenuBar();
    void setupActivityLog();
    void applyDarkTheme();

    StateManager& stateManager_;
    MediaManager& mediaManager_;
    BluetoothManager& bluetoothManager_;
    NavigationManager& navigationManager_;
    TelemetryManager& telemetryManager_;
    SettingsManager& settingsManager_;

    QListWidget* navList_;
    QStackedWidget* stackedWidget_;

    DashboardWidget* dashboardWidget_;
    MediaWidget* mediaWidget_;
    BluetoothWidget* bluetoothWidget_;
    NavigationWidget* navigationWidget_;
    TelemetryWidget* telemetryWidget_;
    SettingsWidget* settingsWidget_;

    QDockWidget* logDock_;
    QTextEdit* logTextEdit_;

    QLabel* statusLabel_;
    QLabel* errorLabel_;
};

} // namespace infotainment
