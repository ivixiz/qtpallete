//FILE: QtPallete/qtcontrol.h
#pragma once
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "qtpalette.h"

using ChannelInput = std::tuple<QHBoxLayout*, QLabel*, QLineEdit*>;
struct ChannelConfig {
    QString label;
    QString placeholder;
    bool enabled;
    QString value;
};
std::tuple<QPushButton*, QLineEdit*, std::vector<ChannelInput>>
createTextInputRow(QVBoxLayout *layout);
class ColorTextControls : public QObject {
    Q_OBJECT
public:
    ColorTextControls(QVBoxLayout *layout, ColorPickerWidget *picker, QObject *parent = nullptr);
private:
    std::shared_ptr<bool> guard;
    ColorPickerWidget *picker;
    QPushButton *modeBtn;
    QLineEdit *hexInput;
    std::vector<ChannelInput> inputs;
    void connectChannelInputs(ColorPickerWidget *picker, QLineEdit *A, QLineEdit *R,
                          QLineEdit *G, QLineEdit *B, std::shared_ptr<bool> guard);
    void connectHexInput(ColorPickerWidget *picker, QLineEdit *hexInput, std::shared_ptr<bool> guard);
    void connectModeButton(QPushButton *modeBtn, ColorPickerWidget *picker,
                       const std::vector<ChannelInput> &inputs,
                       std::shared_ptr<bool> guard);
    void connectColorPickerSync(ColorPickerWidget *picker, QLineEdit *hexInput,
                             const std::vector<ChannelInput> &inputs,
                             std::shared_ptr<bool> guard);
    void applyMode(ColorMode mode, const QColor &color);
};