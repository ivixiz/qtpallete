// FILE: QtPalette/screeneyedropper.cpp
#include "screeneyedropper.h"
#include "qtpalette.h"      // ColorPickerWidget declaration
#include <QTimer>
#include <QWidget>
#include <QDebug>
#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xutil.h>      // XGetImage
#include <xcb/xcb.h>
ScreenEyedropper::ScreenEyedropper(ColorPickerWidget *picker, QObject *parent)
  : QObject(parent), m_picker(picker){
    // Install native event filter to catch global ButtonPress via X11
    qApp->installNativeEventFilter(this);
    // Timer to monitor window exit/enter
    m_timer = new QTimer(this);
    m_timer->setInterval(50);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        QWidget *w = m_picker->window();
        QRect bounds = w->frameGeometry();
        QPoint p = QCursor::pos();
        if (!m_active && !bounds.contains(p))
            startEyedropper();
        else if (m_active && bounds.contains(p))
            stopEyedropper();
    });
    m_timer->start();
}
bool ScreenEyedropper::nativeEventFilter(const QByteArray &evt, void *msg, long *res){
    if (!m_active || evt != "xcb_generic_event_t")
        return false;
    auto *xev = static_cast<xcb_generic_event_t *>(msg);
    uint8_t type = xev->response_type & ~0x80;
    if (type == XCB_BUTTON_PRESS) {
        QColor c = grabColorUnderCursor();
        qDebug() << "Color picked:" << c;
        if (c.isValid())
            m_picker->setColor(c);
        stopEyedropper();
        *res = 1;
        return true;
    }
    return false;
}

void ScreenEyedropper::startEyedropper(){
    if (m_active) return;
    m_active = true;
    QApplication::setOverrideCursor(Qt::CrossCursor);
    Display *dpy = QX11Info::display();
    Window root = DefaultRootWindow(dpy);
    if (XGrabPointer(dpy, root, True,
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                     GrabModeAsync, GrabModeAsync,
                     None,
                     XcursorLibraryLoadCursor(dpy, "crosshair"),
                     CurrentTime) != GrabSuccess){
        qWarning() << "XGrabPointer failed";
        m_active = false;
        QApplication::restoreOverrideCursor();
    }
}
void ScreenEyedropper::stopEyedropper(){
    if (!m_active) return;
    m_active = false;
    QApplication::restoreOverrideCursor();
    XUngrabPointer(QX11Info::display(), CurrentTime);
}

QColor ScreenEyedropper::grabColorUnderCursor() const{
    Display *dpy = QX11Info::display();
    Window root = DefaultRootWindow(dpy);
    QPoint pos = QCursor::pos();
    XImage *img = XGetImage(dpy, root, pos.x(), pos.y(), 1, 1,
                            AllPlanes, ZPixmap);
    if (!img) return QColor();
    unsigned long pixel = XGetPixel(img, 0, 0);
    XDestroyImage(img);
    int r = (pixel >> 16) & 0xFF;
    int g = (pixel >> 8) & 0xFF;
    int b =  pixel       & 0xFF;
    return QColor(r, g, b);
}