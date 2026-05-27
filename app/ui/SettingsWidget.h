#pragma once

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include "../../src/core/AppState.h"

namespace infotainment {

class StateManager;

class SettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWidget(StateManager& stateManager, QWidget* parent = nullptr);
    void updateFromState(const AppState& state);

private:
    void setupUI();

    StateManager& stateManager_;
    QTableWidget* settingsTable_;
    QLabel* stateJsonLabel_;
};

} // namespace infotainment
