// FILE: QtPalette/screeneyedropper.h
#ifndef SCREENEYEDROPPER_H
#define SCREENEYEDROPPER_H

#include <QObject>
#include <QColor>
#include <QCursor>
#include <QApplication>
#include <QX11Info>
#include <QAbstractNativeEventFilter>

class ColorPickerWidget;
class QTimer;

class ScreenEyedropper : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT
public:
    explicit ScreenEyedropper(ColorPickerWidget *picker, QObject *parent = nullptr);
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

private:
    void startEyedropper();
    void stopEyedropper();
    QColor grabColorUnderCursor() const;

    ColorPickerWidget *m_picker;
    QTimer *m_timer = nullptr;
    bool m_active = false;
};

#endif // SCREENEYEDROPPER_H