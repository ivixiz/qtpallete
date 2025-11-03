
//FILE: QtPallete/qtcontrol.cpp
#include "qtcontrol.h"
#include "control_by_wheel.h"
ColorTextControls::ColorTextControls(QVBoxLayout *layout, ColorPickerWidget *picker, QObject *parent)
    : QObject(parent), guard(std::make_shared<bool>(false)), picker(picker){
    auto [modeBtn, hexInput, inputs] = createTextInputRow(layout);
    // 0: A/K/-, 1: R/C/H, 2: G/M/S, 3: B/Y/V
    auto [layoutA, labelA, A] = inputs[0];
    auto [layoutR, labelR, R] = inputs[1];
    auto [layoutG, labelG, G] = inputs[2];
    auto [layoutB, labelB, B] = inputs[3];
    connectChannelInputs(picker, A, R, G, B, guard);
    connectModeButton(modeBtn, picker, inputs, guard);
    connectColorPickerSync(picker, hexInput, inputs, guard);
    connectHexInput(picker, hexInput, guard);
}

void setChannelState(QLineEdit *input, QLabel *label, const QString &labelText,
                     const QString &placeholder, bool enabled, const QString &text = {}) {
    label->setText(labelText);
    input->setPlaceholderText(placeholder);
    input->setEnabled(enabled);
    if (!text.isNull())            
        input->setText(text);
    if (!enabled)
        input->clear();
}
void applyChannelConfigs(const std::vector<ChannelConfig>& configs,
                         const std::vector<ChannelInput>& inputs) {
    for (int i = 0; i < 4; ++i) {
        auto &[layout, labelWidget, lineEdit] = inputs[i];
        const auto &conf = configs[i];
        setChannelState(lineEdit, labelWidget, conf.label, conf.placeholder,  conf.enabled, conf.value);
    }
}

std::tuple<QHBoxLayout*, QLabel*, QLineEdit*> createChannelInput(const QString& labelText) {
    auto *layout = new QHBoxLayout;
    auto *label = new QLabel(labelText);
    auto *input = new WheelLineEdit;  // каст в createTextInputRow, если нужно

    input->setFixedWidth(30);
    input->setMaxLength(3);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label);
    layout->addWidget(input);

    return {layout, label, input};
}
std::tuple<QPushButton*, QLineEdit*, std::vector<ChannelInput>>
createTextInputRow(QVBoxLayout *layout) {
    auto *inputLayout = new QHBoxLayout;
    layout->addLayout(inputLayout);

    auto *modeBtn = new QPushButton("ARGB");
    inputLayout->addWidget(modeBtn);

    auto *hexInput = new QLineEdit;
    hexInput->setFixedSize(85, 20);
    hexInput->setMaxLength(9);
    hexInput->setPlaceholderText("#HEX|ARGB");
    inputLayout->addWidget(hexInput);

    std::vector<ChannelInput> inputs;

    auto inputA = createChannelInput("A:"); inputs.push_back(inputA);
    auto inputR = createChannelInput("R:"); inputs.push_back(inputR);
    auto inputG = createChannelInput("G:"); inputs.push_back(inputG);
    auto inputB = createChannelInput("B:"); inputs.push_back(inputB);

    for (auto &[layout, _, __] : inputs)
        inputLayout->addLayout(layout);

    return {modeBtn, hexInput, inputs};
}
void ColorTextControls::connectChannelInputs(ColorPickerWidget *picker, QLineEdit *A, QLineEdit *R,
                          QLineEdit *G, QLineEdit *B, std::shared_ptr<bool> guard) {
    auto updateColor = [=]() {
    if (*guard) return;
    *guard = true;
    bool okR, okG, okB, okA;
    int r, g, b, a;
    QColor color;
    auto clampAndFix = [](QLineEdit *edit, int min, int max, bool &ok) -> int {
        int value = edit->text().toInt(&ok);
        if (!ok || value < min || value > max) {
            value = max;
            edit->setText(QString::number(value));  // Обновить в поле
        }
        return value;
    };
    switch (currentColorMode) {
        case ColorMode::ARGB:
            r = clampAndFix(R, 0, 255, okR);
            g = clampAndFix(G, 0, 255, okG);
            b = clampAndFix(B, 0, 255, okB);
            a = clampAndFix(A, 0, 255, okA);
            picker->setColor(QColor(r, g, b, a));
            break;
        case ColorMode::HSV: {
            int h = clampAndFix(R, 0, 360, okR);
            int s = clampAndFix(G, 0, 100, okG);
            int v = clampAndFix(B, 0, 100, okB);
            color.setHsvF(h / 360.0, s / 100.0, v / 100.0);
            color.setAlpha(picker->selectedColor().alpha());
            picker->setColor(color);
            break;
        }
        case ColorMode::HSL: {
            int h = clampAndFix(R, 0, 360, okR);
            int s = clampAndFix(G, 0, 100, okG);
            int l = clampAndFix(B, 0, 100, okB);
            color.setHslF(h / 360.0, s / 100.0, l / 100.0);
            color.setAlpha(picker->selectedColor().alpha());
            picker->setColor(color);
            break;
        }
        case ColorMode::CMYK: {
            r = clampAndFix(R, 0, 255, okR);
            g = clampAndFix(G, 0, 255, okG);
            b = clampAndFix(B, 0, 255, okB);
            a = clampAndFix(A, 0, 255, okA);

            double c = r / 255.0;
            double m = g / 255.0;
            double y = b / 255.0;
            double kf = a / 255.0;

            double Rf = (1.0 - c) * (1.0 - kf);
            double Gf = (1.0 - m) * (1.0 - kf);
            double Bf = (1.0 - y) * (1.0 - kf);

            QColor color = QColor::fromRgbF(Rf, Gf, Bf);
            color.setAlpha(picker->selectedColor().alpha());
            picker->setColor(color);
            break;
        }
    }

    for (auto &[layout, label, input] : inputs) {
        if (auto *wInput = dynamic_cast<WheelLineEdit*>(input)) {
            wInput->setRange(0, 255);
            wInput->setStep(1);
        }
    }

    *guard = false;
};
    for (auto *w : {A, R, G, B}) {
        QObject::connect(w, &QLineEdit::textChanged,
            [=](const QString &) { updateColor(); });
    }
}
void ColorTextControls::connectModeButton(QPushButton *modeBtn, ColorPickerWidget *picker,
                       const std::vector<ChannelInput> &inputs,
                       std::shared_ptr<bool> guard) {
    QObject::connect(modeBtn, &QPushButton::clicked, [=]() mutable {
        static int modeIndex = 0;
        modeIndex = (modeIndex + 1) % ModeOrder.size();
        currentColorMode = ModeOrder[modeIndex];

        const QColor c = picker->selectedColor();
        *guard = true;
        switch (currentColorMode) {
            case ColorMode::ARGB:
                modeBtn->setText("ARGB");
                applyChannelConfigs({
                    {"A:", "0-255", true, QString::number(c.alpha())},
                    {"R:", "0-255", true, QString::number(c.red())},
                    {"G:", "0-255", true, QString::number(c.green())},
                    {"B:", "0-255", true, QString::number(c.blue())}
                }, inputs);
                break;
            case ColorMode::CMYK: {
                modeBtn->setText("CMYK");
                double r = c.redF(), g = c.greenF(), b = c.blueF();
                double k = 1 - std::max({r, g, b});
                double c_ = (1 - r - k) / (1 - k + 1e-6);
                double m  = (1 - g - k) / (1 - k + 1e-6);
                double y  = (1 - b - k) / (1 - k + 1e-6);
                applyChannelConfigs({
                    {"K:", "0-255", true, QString::number(int(k * 255))},
                    {"C:", "0-255", true, QString::number(int(c_* 255))},
                    {"M:", "0-255", true, QString::number(int(m * 255))},
                    {"Y:", "0-255", true, QString::number(int(y * 255))}
                }, inputs);
                break;
            }
            case ColorMode::HSV: {
                modeBtn->setText("HSV");
                double h, s, v;
                c.getHsvF(&h, &s, &v);
                applyChannelConfigs({
                    {"",    "—",     false, ""},
                    {"H:", "360°",    true, QString::number(int(h * 360))},
                    {"S:", "100%",    true, QString::number(int(s * 100))},
                    {"V:", "100%",    true, QString::number(int(v * 100))}
                }, inputs);
                break;
            }
            case ColorMode::HSL: {
                modeBtn->setText("HSL");
                double h, s, l;
                c.getHslF(&h, &s, &l);
                
                applyChannelConfigs({
                    {"",    "—",     false, ""},
                    {"H:", "360°",    true, QString::number(int(h * 360))},
                    {"S:", "100%",    true, QString::number(int(s * 100))},
                    {"L:", "100%",    true, QString::number(int(l * 100))}
                }, inputs);
                break;
            }
        }

        *guard = false;
        emit picker->colorChanged(c);
    });
}
void ColorTextControls::connectColorPickerSync(ColorPickerWidget *picker, QLineEdit *hexInput,
                             const std::vector<ChannelInput> &inputs,
                             std::shared_ptr<bool> guard) {
    QObject::connect(picker, &ColorPickerWidget::colorChanged,
        [=](const QColor &c) {
            if (*guard) return;
            *guard = true;
            auto [layoutA, labelA, A] = inputs[0];
            auto [layoutR, labelR, R] = inputs[1];
            auto [layoutG, labelG, G] = inputs[2];
            auto [layoutB, labelB, B] = inputs[3];
            QString hex = QString("#%1%2%3%4")
                .arg(c.alpha(), 2, 16, QChar('0'))
                .arg(c.red(),   2, 16, QChar('0'))
                .arg(c.green(), 2, 16, QChar('0'))
                .arg(c.blue(),  2, 16, QChar('0'))
                .toUpper();
            hexInput->setText(hex);
            switch (currentColorMode) {
                case ColorMode::ARGB:
                    A->setText(QString::number(c.alpha()));
                    R->setText(QString::number(c.red()));
                    G->setText(QString::number(c.green()));
                    B->setText(QString::number(c.blue()));
                    break;
                case ColorMode::CMYK: {
                    double r = c.redF(), g = c.greenF(), b = c.blueF();
                    double k = 1 - std::max({r, g, b});
                    double c_ = (1 - r - k) / (1 - k + 1e-6);
                    double m  = (1 - g - k) / (1 - k + 1e-6);
                    double y  = (1 - b - k) / (1 - k + 1e-6);
                    A->setText(QString::number(int(k * 255)));
                    R->setText(QString::number(int(c_ * 255)));
                    G->setText(QString::number(int(m  * 255)));
                    B->setText(QString::number(int(y  * 255)));
                    break;
                }
                case ColorMode::HSV: {
                    double h, s, v;
                    c.getHsvF(&h, &s, &v);
                    A->clear();
                    R->setText(QString::number(int(h * 360)));
                    G->setText(QString::number(int(s * 100)));
                    B->setText(QString::number(int(v * 100)));
                    break;
                }
                case ColorMode::HSL: {
                    double h, s, l;
                    c.getHslF(&h, &s, &l);
                    A->clear(); //bug fix when returns -1 for black\white|
                    R->setText(QString::number(int((h>0?h:0)*360)));//<<<|
                    G->setText(QString::number(int(s * 100)));
                    B->setText(QString::number(int(l * 100)));
                    break;
                }
            }
            *guard = false;
        });
}
void ColorTextControls::connectHexInput(ColorPickerWidget *picker, QLineEdit *hexInput, std::shared_ptr<bool> guard) {
    QObject::connect(hexInput, &QLineEdit::editingFinished, [picker, hexInput, guard]() {
        if (*guard) return;
        *guard = true;

        QString text = hexInput->text().trimmed();
        if (text.startsWith("#")) text = text.mid(1);
        if (text.length() == 8) {
            bool ok;
            int a = text.mid(0,2).toInt(&ok,16); if (!ok) { *guard=false; return; }
            int r = text.mid(2,2).toInt(&ok,16); if (!ok) { *guard=false; return; }
            int g = text.mid(4,2).toInt(&ok,16); if (!ok) { *guard=false; return; }
            int b = text.mid(6,2).toInt(&ok,16); if (!ok) { *guard=false; return; }
            picker->setColor(QColor(r, g, b, a));
        }

        *guard = false;
    });
}


