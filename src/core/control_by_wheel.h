// FILE: control_by_wheel.h
#pragma once
#include <QLineEdit>
#include <QWheelEvent>

class WheelLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit WheelLineEdit(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    int step = 1;  // шаг при прокрутке
    int minValue = 0;
    int maxValue = 255;

public:
    void setRange(int min, int max) { minValue = min; maxValue = max; }
    void setStep(int s) { step = s; }
};