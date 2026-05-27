#include "DashboardWidget.h"
#include "../../src/core/StateManager.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>

namespace infotainment {

DashboardWidget::DashboardWidget(StateManager& stateManager, QWidget* parent)
    : QWidget(parent), stateManager_(stateManager)
{
    setObjectName("dashboardWidget");
    setupUI();
}

QWidget* DashboardWidget::createCard(const QString& title, const QString& objectName) {
    QFrame* card = new QFrame(this);
    card->setObjectName(objectName);
    card->setStyleSheet(R"(
        QFrame {
            background-color: #F0E6D0;
            border-radius: 12px;
            border: 1px solid #D4C5A9;
            padding: 16px;
        }
        QFrame:hover {
            border-color: #1a1a1a;
            background-color: #EDE3CC;
        }
    )");
    card->setMinimumHeight(120);

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setAlignment(Qt::AlignCenter);
    QLabel* titleLabel = new QLabel(title, card);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #1a1a1a; font-size: 12px; font-weight: bold; text-transform: uppercase; letter-spacing: 1px; border: none; background: transparent;");
    layout->addWidget(titleLabel);

    return card;
}

void DashboardWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* header = new QLabel("Dashboard", this);
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("font-size: 28px; font-weight: bold; color: #1a1a1a; padding-bottom: 8px;");
    mainLayout->addWidget(header);

    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(16);

    // Media card
    auto* mediaCard = createCard("MEDIA", "mediaCard");
    auto* mediaLayout = qobject_cast<QVBoxLayout*>(mediaCard->layout());
    mediaStatusLabel_ = new QLabel("Stopped", this);
    mediaStatusLabel_->setAlignment(Qt::AlignCenter);
    mediaStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    trackLabel_ = new QLabel("No track", this);
    trackLabel_->setAlignment(Qt::AlignCenter);
    trackLabel_->setStyleSheet("color: #666; font-size: 13px; border: none; background: transparent;");
    volumeLabel_ = new QLabel("Vol: 50", this);
    volumeLabel_->setAlignment(Qt::AlignCenter);
    volumeLabel_->setStyleSheet("color: #666; font-size: 13px; border: none; background: transparent;");
    mediaLayout->addWidget(mediaStatusLabel_);
    mediaLayout->addWidget(trackLabel_);
    mediaLayout->addWidget(volumeLabel_);
    grid->addWidget(mediaCard, 0, 0);

    // Bluetooth card
    auto* btCard = createCard("BLUETOOTH", "btCard");
    auto* btLayout = qobject_cast<QVBoxLayout*>(btCard->layout());
    btStatusLabel_ = new QLabel("Disconnected", this);
    btStatusLabel_->setAlignment(Qt::AlignCenter);
    btStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    deviceLabel_ = new QLabel("No device", this);
    deviceLabel_->setAlignment(Qt::AlignCenter);
    deviceLabel_->setStyleSheet("color: #666; font-size: 13px; border: none; background: transparent;");
    callStatusLabel_ = new QLabel("Call: Idle", this);
    callStatusLabel_->setAlignment(Qt::AlignCenter);
    callStatusLabel_->setStyleSheet("color: #666; font-size: 13px; border: none; background: transparent;");
    btLayout->addWidget(btStatusLabel_);
    btLayout->addWidget(deviceLabel_);
    btLayout->addWidget(callStatusLabel_);
    grid->addWidget(btCard, 0, 1);

    // Navigation card
    auto* navCard = createCard("NAVIGATION", "navCard");
    auto* navLayout = qobject_cast<QVBoxLayout*>(navCard->layout());
    navStatusLabel_ = new QLabel("Idle", this);
    navStatusLabel_->setAlignment(Qt::AlignCenter);
    navStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    navLayout->addWidget(navStatusLabel_);
    navLayout->addStretch();
    grid->addWidget(navCard, 0, 2);

    // Speed card
    auto* speedCard = createCard("SPEED", "speedCard");
    auto* speedLayout = qobject_cast<QVBoxLayout*>(speedCard->layout());
    speedLabel_ = new QLabel("0 km/h", this);
    speedLabel_->setAlignment(Qt::AlignCenter);
    speedLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    speedLayout->addWidget(speedLabel_);
    grid->addWidget(speedCard, 1, 0);

    // Fuel card
    auto* fuelCard = createCard("FUEL", "fuelCard");
    auto* fuelLayout = qobject_cast<QVBoxLayout*>(fuelCard->layout());
    fuelLabel_ = new QLabel("100%", this);
    fuelLabel_->setAlignment(Qt::AlignCenter);
    fuelLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    fuelLayout->addWidget(fuelLabel_);
    grid->addWidget(fuelCard, 1, 1);

    // Temperature card
    auto* tempCard = createCard("ENGINE TEMP", "tempCard");
    auto* tempLayout = qobject_cast<QVBoxLayout*>(tempCard->layout());
    tempLabel_ = new QLabel("20 C", this);
    tempLabel_->setAlignment(Qt::AlignCenter);
    tempLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    tempLayout->addWidget(tempLabel_);
    grid->addWidget(tempCard, 1, 2);

    // Notifications card
    auto* notifCard = createCard("NOTIFICATIONS", "notifCard");
    auto* notifLayout = qobject_cast<QVBoxLayout*>(notifCard->layout());
    notifLabel_ = new QLabel("0 pending", this);
    notifLabel_->setAlignment(Qt::AlignCenter);
    notifLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    notifLayout->addWidget(notifLabel_);
    notifLayout->addStretch();
    grid->addWidget(notifCard, 2, 0);

    mainLayout->addLayout(grid);
    mainLayout->addStretch();
}

void DashboardWidget::updateFromState(const AppState& state) {
    // Media
    QString mediaStr = QString::fromStdString(mediaStateToString(state.mediaState)).toUpper();
    mediaStatusLabel_->setText(mediaStr);
    if (state.mediaState == MediaState::Playing) {
        mediaStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #2e7d32; border: none; background: transparent;");
    } else if (state.mediaState == MediaState::Paused) {
        mediaStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #b8860b; border: none; background: transparent;");
    } else {
        mediaStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #666; border: none; background: transparent;");
    }
    trackLabel_->setText(state.track.empty() ? "No track" : QString::fromStdString(state.track));
    volumeLabel_->setText("Vol: " + QString::number(state.volume));

    // Bluetooth
    btStatusLabel_->setText(QString::fromStdString(bluetoothStateToString(state.bluetoothState)).toUpper());
    if (state.bluetoothState == BluetoothState::Connected) {
        btStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #2e7d32; border: none; background: transparent;");
    } else {
        btStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #666; border: none; background: transparent;");
    }
    deviceLabel_->setText(state.connectedDevice.empty() ? "No device" : QString::fromStdString(state.connectedDevice));
    callStatusLabel_->setText("Call: " + QString::fromStdString(callStateToString(state.callState)));

    // Navigation
    if (state.navigationState == NavigationState::AlertActive) {
        navStatusLabel_->setText("ALERT: " + QString::fromStdString(state.navigationMessage));
        navStatusLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #cc0000; border: none; background: transparent;");
    } else {
        navStatusLabel_->setText("Idle");
        navStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #666; border: none; background: transparent;");
    }

    // Telemetry
    speedLabel_->setText(QString::number(state.vehicleSpeed) + " km/h");
    fuelLabel_->setText(QString::number(state.fuelLevel) + "%");
    tempLabel_->setText(QString::number(state.engineTemperature) + " C");

    // Color coding for temperature
    if (state.engineTemperature > 110) {
        tempLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #cc0000; border: none; background: transparent;");
    } else if (state.engineTemperature > 90) {
        tempLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #b8860b; border: none; background: transparent;");
    } else {
        tempLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    }

    // Fuel color coding
    if (state.fuelLevel < 15) {
        fuelLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #cc0000; border: none; background: transparent;");
    } else if (state.fuelLevel < 30) {
        fuelLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #b8860b; border: none; background: transparent;");
    } else {
        fuelLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #1a1a1a; border: none; background: transparent;");
    }

    // Notifications
    notifLabel_->setText(QString::number(state.notificationsPending) + " pending");
}

} // namespace infotainment
