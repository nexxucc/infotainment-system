#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include "../../src/core/AppState.h"

namespace infotainment {

class StateManager;
class NavigationManager;

class MapCanvas : public QWidget {
    Q_OBJECT
public:
    explicit MapCanvas(QWidget* parent = nullptr);
    void setAlertMessage(const QString& msg);
    void setAlertActive(bool active);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString alertMessage_;
    bool alertActive_ = false;
};

class NavigationWidget : public QWidget {
    Q_OBJECT

public:
    explicit NavigationWidget(StateManager& stateManager, NavigationManager& navigationManager, QWidget* parent = nullptr);
    void updateFromState(const AppState& state);

private:
    void setupUI();

    StateManager& stateManager_;
    NavigationManager& navigationManager_;
    QLabel* statusLabel_;
    QLabel* messageLabel_;
    QFrame* alertFrame_;
    MapCanvas* mapCanvas_;
    QPushButton* clearAlertBtn_;
};

} // namespace infotainment
