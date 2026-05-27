#include "MainWindow.h"
#include "DashboardWidget.h"
#include "MediaWidget.h"
#include "BluetoothWidget.h"
#include "NavigationWidget.h"
#include "TelemetryWidget.h"
#include "SettingsWidget.h"
#include "../../src/core/StateManager.h"
#include "../../src/modules/MediaManager.h"
#include "../../src/modules/BluetoothManager.h"
#include "../../src/modules/NavigationManager.h"
#include "../../src/modules/TelemetryManager.h"
#include "../../src/modules/SettingsManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include <QFont>
#include <QDateTime>

namespace infotainment {

MainWindow::MainWindow(StateManager& stateManager,
                       MediaManager& mediaManager,
                       BluetoothManager& bluetoothManager,
                       NavigationManager& navigationManager,
                       TelemetryManager& telemetryManager,
                       SettingsManager& settingsManager,
                       QWidget* parent)
    : QMainWindow(parent), stateManager_(stateManager),
      mediaManager_(mediaManager), bluetoothManager_(bluetoothManager),
      navigationManager_(navigationManager), telemetryManager_(telemetryManager),
      settingsManager_(settingsManager)
{
    setWindowTitle("Infotainment Simulator");
    setMinimumSize(1200, 700);
    resize(1400, 800);

    setupUI();
    setupMenuBar();
    setupActivityLog();
    applyDarkTheme();

    connect(&stateManager_, &StateManager::stateChanged, this, &MainWindow::onStateChanged);
    onStateChanged();
}

void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    navList_ = new QListWidget(this);
    navList_->setObjectName("navSidebar");
    navList_->setFixedWidth(180);
    navList_->setIconSize(QSize(24, 24));
    navList_->addItem("Dashboard");
    navList_->addItem("Media");
    navList_->addItem("Bluetooth");
    navList_->addItem("Navigation");
    navList_->addItem("Telemetry");
    navList_->addItem("Settings");
    navList_->setCurrentRow(0);

    stackedWidget_ = new QStackedWidget(this);
    stackedWidget_->setObjectName("contentStack");

    dashboardWidget_ = new DashboardWidget(stateManager_, this);
    mediaWidget_ = new MediaWidget(stateManager_, mediaManager_, this);
    bluetoothWidget_ = new BluetoothWidget(stateManager_, bluetoothManager_, this);
    navigationWidget_ = new NavigationWidget(stateManager_, navigationManager_, this);
    telemetryWidget_ = new TelemetryWidget(stateManager_, this);
    settingsWidget_ = new SettingsWidget(stateManager_, this);

    stackedWidget_->addWidget(dashboardWidget_);
    stackedWidget_->addWidget(mediaWidget_);
    stackedWidget_->addWidget(bluetoothWidget_);
    stackedWidget_->addWidget(navigationWidget_);
    stackedWidget_->addWidget(telemetryWidget_);
    stackedWidget_->addWidget(settingsWidget_);

    connect(navList_, &QListWidget::currentRowChanged, stackedWidget_, &QStackedWidget::setCurrentIndex);

    mainLayout->addWidget(navList_);
    mainLayout->addWidget(stackedWidget_, 1);

    setCentralWidget(central);

    statusLabel_ = new QLabel("Ready");
    statusLabel_->setObjectName("statusLabel");
    errorLabel_ = new QLabel("");
    errorLabel_->setObjectName("errorLabel");
    errorLabel_->setStyleSheet("color: #cc0000; font-weight: bold;");
    statusBar()->addWidget(statusLabel_, 1);
    statusBar()->addPermanentWidget(errorLabel_);
}

void MainWindow::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Reset State", [this]() { stateManager_.reset(); });
    fileMenu->addSeparator();
    fileMenu->addAction("&Quit", qApp, &QApplication::quit);
}

void MainWindow::setupActivityLog() {
    logDock_ = new QDockWidget("Activity Log", this);
    logDock_->setObjectName("activityLogDock");
    logDock_->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);

    logTextEdit_ = new QTextEdit(this);
    logTextEdit_->setObjectName("activityLog");
    logTextEdit_->setReadOnly(true);
    logTextEdit_->setMaximumHeight(200);
    QFont monoFont("Monospace", 9);
    monoFont.setStyleHint(QFont::TypeWriter);
    logTextEdit_->setFont(monoFont);

    logDock_->setWidget(logTextEdit_);
    addDockWidget(Qt::BottomDockWidgetArea, logDock_);
}

void MainWindow::applyDarkTheme() {
    setStyleSheet(R"(
        QMainWindow {
            background-color: #FAF3E0;
        }
        QMenuBar {
            background-color: #F0E6D0;
            color: #1a1a1a;
            border-bottom: 1px solid #D4C5A9;
            padding: 4px;
        }
        QMenuBar::item:selected {
            background-color: #D4C5A9;
            border-radius: 4px;
        }
        QMenu {
            background-color: #F0E6D0;
            color: #1a1a1a;
            border: 1px solid #D4C5A9;
        }
        QMenu::item:selected {
            background-color: #D4C5A9;
        }
        #navSidebar {
            background-color: #F0E6D0;
            color: #3a3a3a;
            border: none;
            border-right: 1px solid #D4C5A9;
            font-size: 14px;
            padding: 8px 0;
        }
        #navSidebar::item {
            padding: 14px 20px;
            border-radius: 0;
            border-left: 3px solid transparent;
        }
        #navSidebar::item:selected {
            background-color: #FAF3E0;
            color: #1a1a1a;
            border-left: 3px solid #1a1a1a;
            font-weight: bold;
        }
        #navSidebar::item:hover {
            background-color: #EDE3CC;
            color: #1a1a1a;
        }
        #contentStack {
            background-color: #FAF3E0;
        }
        QStackedWidget > QWidget {
            background-color: #FAF3E0;
        }
        QStatusBar {
            background-color: #F0E6D0;
            color: #3a3a3a;
            border-top: 1px solid #D4C5A9;
            padding: 4px;
        }
        QDockWidget {
            color: #1a1a1a;
            titlebar-close-icon: none;
        }
        QDockWidget::title {
            background-color: #F0E6D0;
            color: #1a1a1a;
            padding: 6px;
            border-bottom: 1px solid #D4C5A9;
        }
        #activityLog {
            background-color: #FFF8EC;
            color: #3a3a3a;
            border: none;
            selection-background-color: #EDE3CC;
        }
        QLabel {
            color: #1a1a1a;
        }
        QPushButton {
            background-color: #1a1a1a;
            color: #FAF3E0;
            border: 1px solid #333;
            border-radius: 6px;
            padding: 8px 20px;
            font-size: 13px;
            font-weight: bold;
            min-width: 70px;
        }
        QPushButton:hover {
            background-color: #333;
            border-color: #555;
        }
        QPushButton:pressed {
            background-color: #000;
        }
        QPushButton:disabled {
            background-color: #D4C5A9;
            color: #999;
            border-color: #C0B090;
        }
        QSlider::groove:horizontal {
            height: 6px;
            background: #D4C5A9;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #1a1a1a;
            width: 16px;
            height: 16px;
            margin: -5px 0;
            border-radius: 8px;
        }
        QSlider::sub-page:horizontal {
            background: #1a1a1a;
            border-radius: 3px;
        }
        QProgressBar {
            background-color: #E8DCC8;
            border: none;
            border-radius: 4px;
            height: 12px;
            text-align: center;
            color: #1a1a1a;
            font-size: 10px;
        }
        QProgressBar::chunk {
            border-radius: 4px;
        }
        QGroupBox {
            color: #3a3a3a;
            border: 1px solid #D4C5A9;
            border-radius: 8px;
            margin-top: 12px;
            padding: 16px 12px 12px 12px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 8px;
            color: #1a1a1a;
        }
        QLineEdit {
            background-color: #FFF8EC;
            color: #1a1a1a;
            border: 1px solid #D4C5A9;
            border-radius: 4px;
            padding: 6px 10px;
        }
        QLineEdit:focus {
            border-color: #1a1a1a;
        }
    )");
}

void MainWindow::appendLog(const QString& entry) {
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    logTextEdit_->append("[" + timestamp + "] " + entry);
}

void MainWindow::showError(const QString& error) {
    errorLabel_->setText(error);
    appendLog("ERROR: " + error);
}

void MainWindow::onStateChanged() {
    auto state = stateManager_.getState();
    statusLabel_->setText(
        QString("Media: %1 | BT: %2 | Call: %3 | Nav: %4 | Screen: %5")
            .arg(QString::fromStdString(mediaStateToString(state.mediaState)))
            .arg(QString::fromStdString(bluetoothStateToString(state.bluetoothState)))
            .arg(QString::fromStdString(callStateToString(state.callState)))
            .arg(QString::fromStdString(navigationStateToString(state.navigationState)))
            .arg(QString::fromStdString(activeScreenToString(state.activeScreen)))
    );

    if (!state.lastError.empty()) {
        errorLabel_->setText(QString::fromStdString(state.lastError));
    } else {
        errorLabel_->setText("");
    }

    dashboardWidget_->updateFromState(state);
    mediaWidget_->updateFromState(state);
    bluetoothWidget_->updateFromState(state);
    navigationWidget_->updateFromState(state);
    telemetryWidget_->updateFromState(state);
    settingsWidget_->updateFromState(state);
}

} // namespace infotainment
