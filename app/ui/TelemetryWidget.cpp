#include "TelemetryWidget.h"
#include "../../src/core/StateManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

namespace infotainment {

TelemetryWidget::TelemetryWidget(StateManager& stateManager, QWidget* parent)
    : QWidget(parent), stateManager_(stateManager)
{
    setObjectName("telemetryWidget");
    setupUI();
}

void TelemetryWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* header = new QLabel("Vehicle Telemetry", this);
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("font-size: 28px; font-weight: bold; color: #1a1a1a;");
    mainLayout->addWidget(header);

    auto createGauge = [this](const QString& title, const QString& unit,
                               QLabel*& valueLabel, QProgressBar*& bar,
                               int maxVal, const QString& color) -> QFrame* {
        QFrame* frame = new QFrame(this);
        frame->setStyleSheet("background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 20px;");
        QVBoxLayout* layout = new QVBoxLayout(frame);
        layout->setAlignment(Qt::AlignCenter);

        QLabel* titleLabel = new QLabel(title, this);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("color: #1a1a1a; font-size: 11px; font-weight: bold; letter-spacing: 2px;");
        layout->addWidget(titleLabel);

        valueLabel = new QLabel("0" + unit, this);
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #1a1a1a;");
        layout->addWidget(valueLabel);

        bar = new QProgressBar(this);
        bar->setRange(0, maxVal);
        bar->setValue(0);
        bar->setTextVisible(false);
        bar->setFixedHeight(8);
        bar->setStyleSheet(
            "QProgressBar { background-color: #E8DCC8; border-radius: 4px; }"
            "QProgressBar::chunk { background-color: " + color + "; border-radius: 4px; }");
        layout->addWidget(bar);

        return frame;
    };

    // Speed gauge
    QFrame* speedFrame = createGauge("SPEED", " km/h", speedValue_, speedBar_, 240, "#1a1a1a");
    mainLayout->addWidget(speedFrame);

    // Fuel and Temperature side by side
    QHBoxLayout* bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(16);

    QFrame* fuelFrame = createGauge("FUEL LEVEL", "%", fuelValue_, fuelBar_, 100, "#1a1a1a");
    bottomRow->addWidget(fuelFrame);

    QFrame* tempFrame = createGauge("ENGINE TEMPERATURE", " C", tempValue_, tempBar_, 160, "#1a1a1a");
    tempBar_->setRange(0, 160);
    bottomRow->addWidget(tempFrame);

    mainLayout->addLayout(bottomRow);
    mainLayout->addStretch();
}

void TelemetryWidget::updateFromState(const AppState& state) {
    speedValue_->setText(QString::number(state.vehicleSpeed) + " km/h");
    speedBar_->setValue(state.vehicleSpeed);

    fuelValue_->setText(QString::number(state.fuelLevel) + "%");
    fuelBar_->setValue(state.fuelLevel);

    tempValue_->setText(QString::number(state.engineTemperature) + " C");
    tempBar_->setValue(state.engineTemperature + 20);

    // Speed color coding
    if (state.vehicleSpeed > 180) {
        speedValue_->setStyleSheet("font-size: 48px; font-weight: bold; color: #cc0000;");
        speedBar_->setStyleSheet("QProgressBar { background-color: #E8DCC8; border-radius: 4px; } QProgressBar::chunk { background-color: #cc0000; border-radius: 4px; }");
    } else {
        speedValue_->setStyleSheet("font-size: 48px; font-weight: bold; color: #1a1a1a;");
        speedBar_->setStyleSheet("QProgressBar { background-color: #E8DCC8; border-radius: 4px; } QProgressBar::chunk { background-color: #1a1a1a; border-radius: 4px; }");
    }

    if (state.fuelLevel < 15) {
        fuelValue_->setStyleSheet("font-size: 48px; font-weight: bold; color: #cc0000;");
        fuelBar_->setStyleSheet("QProgressBar { background-color: #E8DCC8; border-radius: 4px; } QProgressBar::chunk { background-color: #cc0000; border-radius: 4px; }");
    } else {
        fuelValue_->setStyleSheet("font-size: 48px; font-weight: bold; color: #1a1a1a;");
        fuelBar_->setStyleSheet("QProgressBar { background-color: #E8DCC8; border-radius: 4px; } QProgressBar::chunk { background-color: #1a1a1a; border-radius: 4px; }");
    }

    if (state.engineTemperature > 110) {
        tempValue_->setStyleSheet("font-size: 48px; font-weight: bold; color: #cc0000;");
        tempBar_->setStyleSheet("QProgressBar { background-color: #E8DCC8; border-radius: 4px; } QProgressBar::chunk { background-color: #cc0000; border-radius: 4px; }");
    } else {
        tempValue_->setStyleSheet("font-size: 48px; font-weight: bold; color: #1a1a1a;");
        tempBar_->setStyleSheet("QProgressBar { background-color: #E8DCC8; border-radius: 4px; } QProgressBar::chunk { background-color: #1a1a1a; border-radius: 4px; }");
    }
}

} // namespace infotainment
