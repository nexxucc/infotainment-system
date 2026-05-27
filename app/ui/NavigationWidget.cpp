#include "NavigationWidget.h"
#include "../../src/core/StateManager.h"
#include "../../src/modules/NavigationManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPainter>
#include <QPushButton>
#include <cmath>

namespace infotainment {

// ---------- MapCanvas ----------

MapCanvas::MapCanvas(QWidget* parent) : QWidget(parent) {
    setMinimumSize(600, 350);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MapCanvas::setAlertMessage(const QString& msg) {
    alertMessage_ = msg;
    update();
}

void MapCanvas::setAlertActive(bool active) {
    alertActive_ = active;
    update();
}

void MapCanvas::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // Background - light terrain
    p.fillRect(rect(), QColor("#E8DCC8"));

    // Draw grid roads
    QPen roadPen(QColor("#D4C5A9"), 1, Qt::DotLine);
    p.setPen(roadPen);
    for (int x = 0; x < w; x += 80) {
        p.drawLine(x, 0, x, h);
    }
    for (int y = 0; y < h; y += 80) {
        p.drawLine(0, y, w, y);
    }

    // Draw main roads
    QPen mainRoad(QColor("#C0B090"), 24);
    mainRoad.setCapStyle(Qt::FlatCap);
    p.setPen(mainRoad);

    // Horizontal main road
    int roadY = h / 2;
    p.drawLine(0, roadY, w, roadY);

    // Vertical main road
    int roadX = w / 2;
    p.drawLine(roadX, 0, roadX, h);

    // Secondary roads
    QPen secRoad(QColor("#C8BC9E"), 14);
    secRoad.setCapStyle(Qt::FlatCap);
    p.setPen(secRoad);
    p.drawLine(0, h / 4, w, h / 4);
    p.drawLine(0, 3 * h / 4, w, 3 * h / 4);
    p.drawLine(w / 4, 0, w / 4, h);
    p.drawLine(3 * w / 4, 0, 3 * w / 4, h);

    // Road markings (center lines)
    QPen marking(QColor("#FAF3E0"), 2, Qt::DashLine);
    p.setPen(marking);
    p.drawLine(0, roadY, w, roadY);
    p.drawLine(roadX, 0, roadX, h);

    // Draw blocks/buildings (simple rectangles)
    QColor blockColor("#D4C5A9");
    p.setPen(Qt::NoPen);
    p.setBrush(blockColor);

    // Top-left block
    p.drawRoundedRect(roadX / 4 + 20, roadY / 4 + 20,
                      roadX / 4 - 10, roadY / 4 - 10, 4, 4);
    // Top-right block
    p.drawRoundedRect(roadX + roadX / 4 + 20, roadY / 4 + 20,
                      roadX / 3 - 10, roadY / 4 - 10, 4, 4);
    // Bottom-left block
    p.drawRoundedRect(roadX / 4 + 20, roadY + roadY / 4 + 20,
                      roadX / 4 - 10, roadY / 4 - 10, 4, 4);

    // Car position (center of intersection)
    int carX = roadX;
    int carY = roadY;

    // Car shadow
    p.setBrush(QColor(0, 0, 0, 30));
    p.drawEllipse(QPoint(carX + 2, carY + 2), 14, 14);

    // Car dot
    p.setBrush(QColor("#1a1a1a"));
    p.drawEllipse(QPoint(carX, carY), 12, 12);

    // Inner circle
    p.setBrush(QColor("#FAF3E0"));
    p.drawEllipse(QPoint(carX, carY), 5, 5);

    // Navigation direction indicator
    if (alertActive_) {
        // Draw direction arrow (pointing right for demo)
        QPen arrowPen(QColor("#cc0000"), 3);
        p.setPen(arrowPen);
        int arrowStartX = carX + 20;
        int arrowEndX = carX + 60;
        p.drawLine(arrowStartX, carY, arrowEndX, carY);
        // Arrowhead
        p.drawLine(arrowEndX, carY, arrowEndX - 10, carY - 8);
        p.drawLine(arrowEndX, carY, arrowEndX - 10, carY + 8);

        // Alert banner at top
        QRect bannerRect(20, 10, w - 40, 36);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(204, 0, 0, 200));
        p.drawRoundedRect(bannerRect, 6, 6);

        p.setPen(QColor("#FAF3E0"));
        QFont alertFont = p.font();
        alertFont.setBold(true);
        alertFont.setPointSize(11);
        p.setFont(alertFont);
        p.drawText(bannerRect, Qt::AlignCenter, alertMessage_);
    }

    // Label
    p.setPen(QColor("#999"));
    QFont labelFont = p.font();
    labelFont.setPointSize(9);
    p.setFont(labelFont);
    p.drawText(QRect(0, h - 24, w, 20), Qt::AlignCenter, "Simulated Map View");
}

// ---------- NavigationWidget ----------

NavigationWidget::NavigationWidget(StateManager& stateManager, NavigationManager& navigationManager, QWidget* parent)
    : QWidget(parent), stateManager_(stateManager), navigationManager_(navigationManager)
{
    setObjectName("navigationWidget");
    setupUI();
}

void NavigationWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 32, 32, 32);
    mainLayout->setSpacing(20);

    QLabel* header = new QLabel("Navigation", this);
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("font-size: 28px; font-weight: bold; color: #1a1a1a;");
    mainLayout->addWidget(header);

    // Alert info bar
    alertFrame_ = new QFrame(this);
    alertFrame_->setObjectName("navAlertFrame");
    alertFrame_->setStyleSheet("QFrame#navAlertFrame { background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 12px; }");
    QHBoxLayout* alertLayout = new QHBoxLayout(alertFrame_);

    statusLabel_ = new QLabel("No active alerts", this);
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #666;");
    alertLayout->addWidget(statusLabel_, 1);

    messageLabel_ = new QLabel("", this);
    messageLabel_->setAlignment(Qt::AlignCenter);
    messageLabel_->setStyleSheet("font-size: 14px; color: #1a1a1a;");
    messageLabel_->setWordWrap(true);
    alertLayout->addWidget(messageLabel_, 1);

    clearAlertBtn_ = new QPushButton("Clear Alert", this);
    clearAlertBtn_->setEnabled(false);
    alertLayout->addWidget(clearAlertBtn_);

    mainLayout->addWidget(alertFrame_);

    // Map canvas
    mapCanvas_ = new MapCanvas(this);
    mapCanvas_->setStyleSheet("border-radius: 12px;");
    mainLayout->addWidget(mapCanvas_, 1);

    // Wire button
    connect(clearAlertBtn_, &QPushButton::clicked, [this]() {
        navigationManager_.endAlert();
    });
}

void NavigationWidget::updateFromState(const AppState& state) {
    if (state.navigationState == NavigationState::AlertActive) {
        statusLabel_->setText("ALERT ACTIVE");
        statusLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #cc0000;");
        messageLabel_->setText(QString::fromStdString(state.navigationMessage));
        alertFrame_->setStyleSheet("QFrame#navAlertFrame { background-color: #F5E0E0; border-radius: 12px; border: 2px solid #cc0000; padding: 12px; }");
        clearAlertBtn_->setEnabled(true);
        mapCanvas_->setAlertActive(true);
        mapCanvas_->setAlertMessage(QString::fromStdString(state.navigationMessage));
    } else {
        statusLabel_->setText("No active alerts");
        statusLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #666;");
        messageLabel_->setText("");
        alertFrame_->setStyleSheet("QFrame#navAlertFrame { background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 12px; }");
        clearAlertBtn_->setEnabled(false);
        mapCanvas_->setAlertActive(false);
        mapCanvas_->setAlertMessage("");
    }
}

} // namespace infotainment
