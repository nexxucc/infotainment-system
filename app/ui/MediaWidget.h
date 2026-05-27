#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include "../../src/core/AppState.h"

namespace infotainment {

class StateManager;
class MediaManager;

class MediaWidget : public QWidget {
    Q_OBJECT

public:
    explicit MediaWidget(StateManager& stateManager, MediaManager& mediaManager, QWidget* parent = nullptr);
    void updateFromState(const AppState& state);

private:
    void setupUI();

    StateManager& stateManager_;
    MediaManager& mediaManager_;
    QLabel* stateLabel_;
    QLabel* trackLabel_;
    QLabel* volumeValueLabel_;
    QSlider* volumeSlider_;
    QPushButton* playBtn_;
    QPushButton* pauseBtn_;
    QPushButton* stopBtn_;
    QPushButton* prevBtn_;
    QPushButton* nextBtn_;
};

} // namespace infotainment
