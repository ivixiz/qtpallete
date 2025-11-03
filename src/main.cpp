//FILE: QtPallete/main.cpp
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include "core/qtpalette.h"
#include "core/qtcontrol.h"
#include "core/recentcolors.h"
#include "core/screeneyedropper.h"

int main(int argc, char *argv[]){
    //QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QApplication app(argc, argv);
    QDialog dialog;
    dialog.setWindowTitle("Color Picker");
   
    QVBoxLayout *layout = new QVBoxLayout(&dialog); // vertical layout inside the dialog
    ColorPickerWidget *picker = new ColorPickerWidget(&dialog); // color picker widget
    RecentColorsBar *recentColors = new RecentColorsBar(&dialog);
    ScreenEyedropper *eyedropper = new ScreenEyedropper(picker, &app);
    picker->installEventFilter(eyedropper);
    recentColors->setFixedHeight(16);
    layout->addWidget(recentColors, 0);
    layout->addWidget(picker);
    
    new ColorTextControls(layout, picker);  // control widget
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox( // OK and Cancel
        QDialogButtonBox::Cancel  | QDialogButtonBox::Ok,
        Qt::Horizontal,
        &dialog
    );
    layout->addWidget(buttonBox);

    QPushButton *okBtn = buttonBox->button(QDialogButtonBox::Ok);
    okBtn->setVisible(false);
    
    dialog.setFixedSize(dialog.sizeHint());
    QObject::connect(picker, &ColorPickerWidget::colorChanged,
                    recentColors, &RecentColorsBar::addColor);
    QObject::connect(recentColors, &RecentColorsBar::colorSelected,
                 picker, &ColorPickerWidget::setColor);

    QObject::connect(buttonBox, &QDialogButtonBox::rejected, 
                 &dialog, &QDialog::reject);
    // Wire up OK/Cancel
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    QObject::connect(picker, &ColorPickerWidget::colorChanged, [okBtn](const QColor &c){
        if (!okBtn->isVisible()) okBtn->setVisible(true); // reveal OK
        // then style it
        QString bg = c.name(QColor::HexArgb);
        int gray = qGray(c.rgb());
        QString fg = (gray < 128 ? "#FFFFFF" : "#000000");
        okBtn->setStyleSheet(QString(R"(
            QPushButton {
            background-color: %1;
            color: %2;
            border: none;
            font: bold 14px;
            }
        )").arg(bg).arg(fg));
    });

    // Optional: inspect the result
    if (dialog.exec() == QDialog::Accepted) {
        QColor c = picker->selectedColor();
        qDebug("User pressed OK: ARGB = %d,%d,%d,%d",
               c.alpha(), c.red(), c.green(), c.blue());
    } else { qDebug("User cancelled."); }
    return 0;
}

    // QObject::connect(picker, &ColorPickerWidget::colorChanged, [=](const QColor &c){
    //     label->setText(QString("RGBA: %1|%2|%3|%4")
    //                    .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha()));

    //     QPalette pal = label->palette();
    //     pal.setColor(QPalette::WindowText, c);
    //     label->setPalette(pal);

    //     QString hex = QString("#%1%2%3%4")
    //         .arg(c.alpha(), 2, 16, QChar('0'))
    //         .arg(c.red(),   2, 16, QChar('0'))
    //         .arg(c.green(), 2, 16, QChar('0'))
    //         .arg(c.blue(),  2, 16, QChar('0')).toUpper();
        
    //     hexInput->setText(hex);
    // });