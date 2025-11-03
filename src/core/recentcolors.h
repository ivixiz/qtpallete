#pragma once
#include <QWidget>
#include <QColor>
#include <QVector>
#include <QPushButton>
#include <QHBoxLayout>

class RecentColorsBar : public QWidget {
    Q_OBJECT
public:
    explicit RecentColorsBar(QWidget *parent = nullptr);

public slots:
    void addColor(const QColor &color);

signals:
    void colorSelected(const QColor &color);

private:
    QHBoxLayout *layout;
    QVector<QColor> recentColors;
    const int maxColors = 20;

    void updateDisplay();
};
