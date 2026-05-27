#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include "../../src/core/AppState.h"

namespace infotainment {

class StateManager;

class DashboardWidget : public QWidget {
    Q_OBJECT

public:
    explicit DashboardWidget(StateManager& stateManager, QWidget* parent = nullptr);
    void updateFromState(const AppState& state);

private:
    void setupUI();
    QWidget* createCard(const QString& title, const QString& objectName);

    StateManager& stateManager_;

    QLabel* mediaStatusLabel_;
    QLabel* trackLabel_;
    QLabel* volumeLabel_;
    QLabel* btStatusLabel_;
    QLabel* deviceLabel_;
    QLabel* callStatusLabel_;
    QLabel* navStatusLabel_;
    QLabel* speedLabel_;
    QLabel* fuelLabel_;
    QLabel* tempLabel_;
    QLabel* notifLabel_;
};

} // namespace infotainment
