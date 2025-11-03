#include "recentcolors.h"

RecentColorsBar::RecentColorsBar(QWidget *parent)
    : QWidget(parent), layout(new QHBoxLayout(this)) {
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(2);
    setLayout(layout);
}
void RecentColorsBar::addColor(const QColor &color) {
    recentColors.removeAll(color);
    recentColors.prepend(color);
    if (recentColors.size() > maxColors)
        recentColors.resize(maxColors);
    updateDisplay();
}
void RecentColorsBar::updateDisplay() {
    // Clear layout
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    for (const QColor &color : recentColors) {
        QPushButton *btn = new QPushButton;
        btn->setFixedSize(16, 16);
        btn->setStyleSheet(QString("background-color: %1; border: 1px solid #444;")
                           .arg(color.name(QColor::HexArgb)));
        layout->addWidget(btn);
        QObject::connect(btn, &QPushButton::clicked, this, [this, color]() {
            emit colorSelected(color);
        });
    }
}
