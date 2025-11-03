// FILE: control_by_wheel.cpp
#include "control_by_wheel.h"

WheelLineEdit::WheelLineEdit(QWidget *parent) : QLineEdit(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

void WheelLineEdit::wheelEvent(QWheelEvent *event) {
    bool ok;
    int value = text().toInt(&ok);
    if (!ok) value = 0;

    int delta = event->angleDelta().y();
    if (delta > 0)
        value += step;
    else
        value -= step;

    if (value < minValue) value = minValue;
    if (value > maxValue) value = maxValue;

    setText(QString::number(value));
    emit editingFinished(); // Обновим цвет, как будто вручную ввели значение

    event->accept();
}