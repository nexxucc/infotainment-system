#include "SettingsWidget.h"
#include "../../src/core/StateManager.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFrame>
#include <QScrollArea>

namespace infotainment {

SettingsWidget::SettingsWidget(StateManager& stateManager, QWidget* parent)
    : QWidget(parent), stateManager_(stateManager)
{
    setObjectName("settingsWidget");
    setupUI();
}

void SettingsWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* header = new QLabel("Settings", this);
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("font-size: 28px; font-weight: bold; color: #1a1a1a;");
    mainLayout->addWidget(header);

    // Settings table
    QFrame* tableFrame = new QFrame(this);
    tableFrame->setStyleSheet("background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 16px;");
    QVBoxLayout* tableLayout = new QVBoxLayout(tableFrame);

    QLabel* tableTitle = new QLabel("SYSTEM SETTINGS", this);
    tableTitle->setAlignment(Qt::AlignCenter);
    tableTitle->setStyleSheet("color: #1a1a1a; font-size: 11px; font-weight: bold; letter-spacing: 2px;");
    tableLayout->addWidget(tableTitle);

    settingsTable_ = new QTableWidget(5, 2, this);
    settingsTable_->setHorizontalHeaderLabels({"Setting", "Value"});
    settingsTable_->horizontalHeader()->setStretchLastSection(true);
    settingsTable_->verticalHeader()->setVisible(false);
    settingsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    settingsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    settingsTable_->setStyleSheet(R"(
        QTableWidget {
            background-color: #FFF8EC;
            color: #1a1a1a;
            gridline-color: #D4C5A9;
            border: none;
        }
        QHeaderView::section {
            background-color: #E8DCC8;
            color: #1a1a1a;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        QTableWidget::item {
            padding: 8px;
        }
        QTableWidget::item:selected {
            background-color: #EDE3CC;
        }
    )");

    QStringList keys = {"theme", "language", "units", "brightness", "auto_connect_bluetooth"};
    QStringList values = {"dark", "en", "metric", "80", "true"};
    for (int i = 0; i < keys.size(); i++) {
        settingsTable_->setItem(i, 0, new QTableWidgetItem(keys[i]));
        settingsTable_->setItem(i, 1, new QTableWidgetItem(values[i]));
    }

    tableLayout->addWidget(settingsTable_);
    mainLayout->addWidget(tableFrame);

    // State JSON display
    QFrame* stateFrame = new QFrame(this);
    stateFrame->setStyleSheet("background-color: #F0E6D0; border-radius: 12px; border: 1px solid #D4C5A9; padding: 16px;");
    QVBoxLayout* stateLayout = new QVBoxLayout(stateFrame);

    QLabel* stateTitle = new QLabel("APPLICATION STATE (JSON)", this);
    stateTitle->setAlignment(Qt::AlignCenter);
    stateTitle->setStyleSheet("color: #1a1a1a; font-size: 11px; font-weight: bold; letter-spacing: 2px;");
    stateLayout->addWidget(stateTitle);

    stateJsonLabel_ = new QLabel("", this);
    stateJsonLabel_->setStyleSheet("color: #3a3a3a; font-family: monospace; font-size: 12px; padding: 8px;");
    stateJsonLabel_->setWordWrap(true);
    stateJsonLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    stateJsonLabel_->setAlignment(Qt::AlignLeft);
    stateLayout->addWidget(stateJsonLabel_);

    mainLayout->addWidget(stateFrame);
    mainLayout->addStretch();
}

void SettingsWidget::updateFromState(const AppState& state) {
    auto json = state.toJson();
    stateJsonLabel_->setText(QString::fromStdString(json.dump(2)));
}

} // namespace infotainment
