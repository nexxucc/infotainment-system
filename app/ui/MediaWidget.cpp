#include "MediaWidget.h"
#include "../../src/core/StateManager.h"
#include "../../src/modules/MediaManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace infotainment {

MediaWidget::MediaWidget(StateManager& stateManager, MediaManager& mediaManager, QWidget* parent)
    : QWidget(parent), stateManager_(stateManager), mediaManager_(mediaManager)
{
    setObjectName("mediaWidget");
    setupUI();
}

void MediaWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 32, 32, 32);
    mainLayout->setSpacing(24);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* header = new QLabel("Media Player", this);
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("font-size: 28px; font-weight: bold; color: #1a1a1a;");
    mainLayout->addWidget(header);

    // Now playing area
    QFrame* nowPlaying = new QFrame(this);
    nowPlaying->setStyleSheet("background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 24px;");
    nowPlaying->setMinimumWidth(500);
    QVBoxLayout* npLayout = new QVBoxLayout(nowPlaying);
    npLayout->setAlignment(Qt::AlignCenter);

    QLabel* npTitle = new QLabel("NOW PLAYING", this);
    npTitle->setAlignment(Qt::AlignCenter);
    npTitle->setStyleSheet("color: #1a1a1a; font-size: 11px; font-weight: bold; letter-spacing: 2px;");
    npLayout->addWidget(npTitle);

    trackLabel_ = new QLabel("No track selected", this);
    trackLabel_->setAlignment(Qt::AlignCenter);
    trackLabel_->setStyleSheet("font-size: 24px; font-weight: bold; color: #1a1a1a; padding: 8px 0;");
    npLayout->addWidget(trackLabel_);

    stateLabel_ = new QLabel("STOPPED", this);
    stateLabel_->setAlignment(Qt::AlignCenter);
    stateLabel_->setStyleSheet("font-size: 14px; color: #666;");
    npLayout->addWidget(stateLabel_);

    mainLayout->addWidget(nowPlaying, 0, Qt::AlignCenter);

    // Controls
    QHBoxLayout* controls = new QHBoxLayout();
    controls->setSpacing(12);
    controls->addStretch();

    prevBtn_ = new QPushButton("Previous", this);
    controls->addWidget(prevBtn_);

    playBtn_ = new QPushButton("Play", this);
    playBtn_->setStyleSheet("padding: 10px 28px; font-size: 15px;");
    controls->addWidget(playBtn_);

    pauseBtn_ = new QPushButton("Pause", this);
    controls->addWidget(pauseBtn_);

    stopBtn_ = new QPushButton("Stop", this);
    controls->addWidget(stopBtn_);

    nextBtn_ = new QPushButton("Next", this);
    controls->addWidget(nextBtn_);

    controls->addStretch();
    mainLayout->addLayout(controls);

    // Volume
    QFrame* volFrame = new QFrame(this);
    volFrame->setStyleSheet("background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 16px;");
    volFrame->setMinimumWidth(500);
    QHBoxLayout* volLayout = new QHBoxLayout(volFrame);

    QLabel* volIcon = new QLabel("Volume", this);
    volIcon->setStyleSheet("font-size: 13px; font-weight: bold; color: #1a1a1a;");
    volLayout->addWidget(volIcon);

    volumeSlider_ = new QSlider(Qt::Horizontal, this);
    volumeSlider_->setRange(0, 100);
    volumeSlider_->setValue(50);
    volLayout->addWidget(volumeSlider_, 1);

    volumeValueLabel_ = new QLabel("50", this);
    volumeValueLabel_->setFixedWidth(50);
    volumeValueLabel_->setAlignment(Qt::AlignCenter);
    volumeValueLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a1a1a;");
    volLayout->addWidget(volumeValueLabel_);

    mainLayout->addWidget(volFrame, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    // Wire buttons
    connect(playBtn_, &QPushButton::clicked, [this]() {
        QString track = trackLabel_->text();
        if (track == "No track selected") track = "Default Track.mp3";
        mediaManager_.play(track.toStdString());
    });
    connect(pauseBtn_, &QPushButton::clicked, [this]() {
        mediaManager_.pause();
    });
    connect(stopBtn_, &QPushButton::clicked, [this]() {
        mediaManager_.stop();
    });
    connect(prevBtn_, &QPushButton::clicked, [this]() {
        mediaManager_.previous();
    });
    connect(nextBtn_, &QPushButton::clicked, [this]() {
        mediaManager_.next();
    });
    connect(volumeSlider_, &QSlider::valueChanged, [this](int val) {
        volumeValueLabel_->setText(QString::number(val));
        mediaManager_.setVolume(val);
    });
}

void MediaWidget::updateFromState(const AppState& state) {
    trackLabel_->setText(state.track.empty() ? "No track selected" : QString::fromStdString(state.track));

    QString stateStr = QString::fromStdString(mediaStateToString(state.mediaState)).toUpper();
    stateLabel_->setText(stateStr);

    if (state.mediaState == MediaState::Playing) {
        stateLabel_->setStyleSheet("font-size: 14px; color: #2e7d32; font-weight: bold;");
    } else if (state.mediaState == MediaState::Paused) {
        stateLabel_->setStyleSheet("font-size: 14px; color: #b8860b; font-weight: bold;");
    } else {
        stateLabel_->setStyleSheet("font-size: 14px; color: #666;");
    }

    volumeSlider_->blockSignals(true);
    volumeSlider_->setValue(state.volume);
    volumeSlider_->blockSignals(false);
    volumeValueLabel_->setText(QString::number(state.volume));
}

} // namespace infotainment
