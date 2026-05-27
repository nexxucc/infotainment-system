#pragma once

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include "../../src/core/AppState.h"

namespace infotainment {

class StateManager;

class TelemetryWidget : public QWidget {
    Q_OBJECT

public:
    explicit TelemetryWidget(StateManager& stateManager, QWidget* parent = nullptr);
    void updateFromState(const AppState& state);

private:
    void setupUI();

    StateManager& stateManager_;
    QLabel* speedValue_;
    QLabel* fuelValue_;
    QLabel* tempValue_;
    QProgressBar* speedBar_;
    QProgressBar* fuelBar_;
    QProgressBar* tempBar_;
};

} // namespace infotainment
