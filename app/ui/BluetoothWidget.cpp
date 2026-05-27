#include "BluetoothWidget.h"
#include "../../src/core/StateManager.h"
#include "../../src/modules/BluetoothManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QInputDialog>

namespace infotainment {

BluetoothWidget::BluetoothWidget(StateManager& stateManager, BluetoothManager& bluetoothManager, QWidget* parent)
    : QWidget(parent), stateManager_(stateManager), bluetoothManager_(bluetoothManager)
{
    setObjectName("bluetoothWidget");
    setupUI();
}

void BluetoothWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 32, 32, 32);
    mainLayout->setSpacing(24);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* header = new QLabel("Bluetooth", this);
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("font-size: 28px; font-weight: bold; color: #1a1a1a;");
    mainLayout->addWidget(header);

    // Connection status
    QFrame* statusFrame = new QFrame(this);
    statusFrame->setStyleSheet("background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 24px;");
    statusFrame->setMinimumWidth(500);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusFrame);
    statusLayout->setAlignment(Qt::AlignCenter);

    QLabel* connTitle = new QLabel("CONNECTION STATUS", this);
    connTitle->setAlignment(Qt::AlignCenter);
    connTitle->setStyleSheet("color: #1a1a1a; font-size: 11px; font-weight: bold; letter-spacing: 2px;");
    statusLayout->addWidget(connTitle);

    statusLabel_ = new QLabel("Disconnected", this);
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #666; padding: 4px 0;");
    statusLayout->addWidget(statusLabel_);

    deviceLabel_ = new QLabel("No device connected", this);
    deviceLabel_->setAlignment(Qt::AlignCenter);
    deviceLabel_->setStyleSheet("font-size: 14px; color: #666;");
    statusLayout->addWidget(deviceLabel_);

    QHBoxLayout* btBtns = new QHBoxLayout();
    btBtns->setAlignment(Qt::AlignCenter);
    connectBtn_ = new QPushButton("Connect", this);
    disconnectBtn_ = new QPushButton("Disconnect", this);
    disconnectBtn_->setStyleSheet("background-color: #8b2525; color: #FAF3E0; border-color: #6b1515; padding: 8px 20px; min-width: 70px; font-size: 13px; font-weight: bold; border-radius: 6px;");
    btBtns->addWidget(connectBtn_);
    btBtns->addWidget(disconnectBtn_);
    statusLayout->addLayout(btBtns);

    mainLayout->addWidget(statusFrame, 0, Qt::AlignCenter);

    // Call section
    callOverlay_ = new QFrame(this);
    callOverlay_->setObjectName("callOverlay");
    callOverlay_->setStyleSheet("QFrame#callOverlay { background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 24px; }");
    callOverlay_->setMinimumWidth(500);
    QVBoxLayout* callLayout = new QVBoxLayout(callOverlay_);
    callLayout->setAlignment(Qt::AlignCenter);

    QLabel* callTitle = new QLabel("PHONE CALL", this);
    callTitle->setAlignment(Qt::AlignCenter);
    callTitle->setStyleSheet("color: #1a1a1a; font-size: 11px; font-weight: bold; letter-spacing: 2px;");
    callLayout->addWidget(callTitle);

    callStatusLabel_ = new QLabel("Idle", this);
    callStatusLabel_->setAlignment(Qt::AlignCenter);
    callStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a1a1a; padding: 4px 0;");
    callLayout->addWidget(callStatusLabel_);

    callerLabel_ = new QLabel("", this);
    callerLabel_->setAlignment(Qt::AlignCenter);
    callerLabel_->setStyleSheet("font-size: 16px; color: #666;");
    callLayout->addWidget(callerLabel_);

    QHBoxLayout* callBtns = new QHBoxLayout();
    callBtns->setAlignment(Qt::AlignCenter);
    acceptCallBtn_ = new QPushButton("Accept", this);
    acceptCallBtn_->setStyleSheet("background-color: #2e7d32; color: #FAF3E0; border-color: #1b5e20; padding: 8px 20px; min-width: 70px; font-size: 13px; font-weight: bold; border-radius: 6px;");
    rejectCallBtn_ = new QPushButton("Reject", this);
    rejectCallBtn_->setStyleSheet("background-color: #8b2525; color: #FAF3E0; border-color: #6b1515; padding: 8px 20px; min-width: 70px; font-size: 13px; font-weight: bold; border-radius: 6px;");
    endCallBtn_ = new QPushButton("End Call", this);
    endCallBtn_->setStyleSheet("background-color: #8b2525; color: #FAF3E0; border-color: #6b1515; padding: 8px 20px; min-width: 70px; font-size: 13px; font-weight: bold; border-radius: 6px;");
    callBtns->addWidget(acceptCallBtn_);
    callBtns->addWidget(rejectCallBtn_);
    callBtns->addWidget(endCallBtn_);
    callLayout->addLayout(callBtns);

    mainLayout->addWidget(callOverlay_, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    // Wire buttons
    connect(connectBtn_, &QPushButton::clicked, [this]() {
        bool ok;
        QString device = QInputDialog::getText(this, "Connect Bluetooth",
                                               "Device name:", QLineEdit::Normal,
                                               "My Phone", &ok);
        if (ok && !device.isEmpty()) {
            bluetoothManager_.connect(device.toStdString());
        }
    });
    connect(disconnectBtn_, &QPushButton::clicked, [this]() {
        bluetoothManager_.disconnect();
    });
    connect(acceptCallBtn_, &QPushButton::clicked, [this]() {
        bluetoothManager_.acceptCall();
    });
    connect(rejectCallBtn_, &QPushButton::clicked, [this]() {
        bluetoothManager_.rejectCall();
    });
    connect(endCallBtn_, &QPushButton::clicked, [this]() {
        bluetoothManager_.endCall();
    });
}

void BluetoothWidget::updateFromState(const AppState& state) {
    QString btStr = QString::fromStdString(bluetoothStateToString(state.bluetoothState)).toUpper();
    statusLabel_->setText(btStr);
    if (state.bluetoothState == BluetoothState::Connected) {
        statusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #2e7d32; padding: 4px 0;");
        deviceLabel_->setText(QString::fromStdString(state.connectedDevice));
    } else if (state.bluetoothState == BluetoothState::Pairing) {
        statusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #b8860b; padding: 4px 0;");
        deviceLabel_->setText("Pairing...");
    } else {
        statusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #666; padding: 4px 0;");
        deviceLabel_->setText("No device connected");
    }

    connectBtn_->setEnabled(state.bluetoothState == BluetoothState::Disconnected);
    disconnectBtn_->setEnabled(state.bluetoothState != BluetoothState::Disconnected);

    QString callStr = QString::fromStdString(callStateToString(state.callState)).toUpper();
    callStatusLabel_->setText(callStr);

    if (state.callState == CallState::Ringing) {
        callStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #b8860b; padding: 4px 0;");
        callerLabel_->setText(QString::fromStdString(state.currentCaller));
        callOverlay_->setStyleSheet("QFrame#callOverlay { background-color: #F5ECD0; border-radius: 12px; border: 2px solid #b8860b; padding: 24px; }");
    } else if (state.callState == CallState::Active) {
        callStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #2e7d32; padding: 4px 0;");
        callerLabel_->setText(QString::fromStdString(state.currentCaller));
        callOverlay_->setStyleSheet("QFrame#callOverlay { background-color: #ECF5E0; border-radius: 12px; border: 2px solid #2e7d32; padding: 24px; }");
    } else {
        callStatusLabel_->setStyleSheet("font-size: 22px; font-weight: bold; color: #666; padding: 4px 0;");
        callerLabel_->setText("");
        callOverlay_->setStyleSheet("QFrame#callOverlay { background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 24px; }");
    }

    acceptCallBtn_->setEnabled(state.callState == CallState::Ringing);
    rejectCallBtn_->setEnabled(state.callState == CallState::Ringing);
    endCallBtn_->setEnabled(state.callState == CallState::Active || state.callState == CallState::Ringing);
}

} // namespace infotainment
