#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "../../src/core/AppState.h"

namespace infotainment {

class StateManager;
class BluetoothManager;

class BluetoothWidget : public QWidget {
    Q_OBJECT

public:
    explicit BluetoothWidget(StateManager& stateManager, BluetoothManager& bluetoothManager, QWidget* parent = nullptr);
    void updateFromState(const AppState& state);

private:
    void setupUI();

    StateManager& stateManager_;
    BluetoothManager& bluetoothManager_;
    QLabel* statusLabel_;
    QLabel* deviceLabel_;
    QLabel* callStatusLabel_;
    QLabel* callerLabel_;
    QPushButton* connectBtn_;
    QPushButton* disconnectBtn_;
    QPushButton* acceptCallBtn_;
    QPushButton* rejectCallBtn_;
    QPushButton* endCallBtn_;
    QFrame* callOverlay_;
};

} // namespace infotainment
