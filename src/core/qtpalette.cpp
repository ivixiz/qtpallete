//FILE: QtPallete/qtpalette.cpp
#include "qtpalette.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QtMath>

ColorMode currentColorMode = ColorMode::ARGB;
ColorPickerWidget::ColorPickerWidget(QWidget *parent)
    : QWidget(parent){
    setMinimumSize(300, 200);
}
QRect  ColorPickerWidget::hueRect() const {
    return QRect(width() - 30, 10, 20, height() - 20);
}
QRect  ColorPickerWidget::alphaRect() const {
    return QRect(width() - 60, 10, 20, height() - 20);
}
QColor ColorPickerWidget::selectedColor() const{
    QColor c = QColor::fromHsvF(hue / 360.0, sat, val);
    c.setAlphaF(alpha);
    return c;
}
QRect  ColorPickerWidget::colorRect() const{
    return QRect(10, 10, width() - 80, height() - 20);
}
void   ColorPickerWidget::paintEvent(QPaintEvent *){
    QPainter p(this);
    QRect cRect = colorRect();
    QRect hRect = hueRect();
    QRect aRect = alphaRect();

    hue = hue >0? hue:0;
    // 1. Draw color square (Saturation vs Value)
    QImage svImage(cRect.size(), QImage::Format_RGB32);
    for (int y = 0; y < cRect.height(); ++y) {
        for (int x = 0; x < cRect.width(); ++x) {
            float s = x / float(cRect.width() - 1);
            float v = 1.0 - y / float(cRect.height() - 1);
            QColor col = QColor::fromHsvF(hue / 360.0, s, v);
            svImage.setPixel(x, y, col.rgb());
        }
    }
    
    // 2. Draw hue bar (vertical rainbow)
    QImage hueImage(hRect.size(), QImage::Format_RGB32);
    for (int y = 0; y < hRect.height(); ++y) {
        float h = (1.0 - y / float(hRect.height() - 1)) * 360.0;
        QColor col = QColor::fromHsvF(h / 360.0, 1.0, 1.0);
        for (int x = 0; x < hRect.width(); ++x)
            hueImage.setPixel(x, y, col.rgb());
    }
    

    // 3. Draw alpha bar: top = opaque, bottom = transparent
    QImage alphaImage(aRect.size(), QImage::Format_ARGB32);
    for (int y = 0; y < aRect.height(); ++y) {
        float f = 1.0 - y / float(aRect.height() - 1);
        QColor base = QColor::fromHsvF(hue / 360.0, sat, val);
        base.setAlphaF(f);
        for (int x = 0; x < aRect.width(); ++x) {
            // Шахматный фон
            bool checker = ((x / 4 + y / 4) % 2) == 0;
            QColor bg = checker ? QColor(200, 200, 200) : QColor(255, 255, 255);

            // Альфа поверх шахматки
            QColor mixed;
            mixed.setRedF(base.alphaF() * base.redF() + (1.0 - base.alphaF()) * bg.redF());
            mixed.setGreenF(base.alphaF() * base.greenF() + (1.0 - base.alphaF()) * bg.greenF());
            mixed.setBlueF(base.alphaF() * base.blueF() + (1.0 - base.alphaF()) * bg.blueF());
            mixed.setAlpha(255);

            alphaImage.setPixelColor(x, y, mixed);
        }
    }

    // 1. Draw color square (Saturation vs Value)
    p.drawImage(cRect.topLeft(), svImage);
    // 2. Draw alpha bar
    p.drawImage(aRect.topLeft(), alphaImage);
    // 3. Draw hue bar
    p.drawImage(hRect.topLeft(), hueImage);

    // 3. Draw selectors
    int sx = cRect.left() + sat * cRect.width();
    int sy = cRect.top() + (1.0 - val) * cRect.height();
    p.setPen(Qt::black);
    p.drawEllipse(QPoint(sx, sy), 5, 5);

    // 4. Draw alpha selector
    int ay = aRect.top() + (1.0 - alpha) * aRect.height();
    p.setPen(Qt::black);
    p.drawRect(aRect.left() - 2, ay - 2, aRect.width() + 4, 4);

    int hy = hRect.top() + (1.0 - hue / 360.0) * hRect.height();
    p.drawRect(hRect.left() - 2, hy - 2, hRect.width() + 4, 4);
}
void   ColorPickerWidget::mousePressEvent(QMouseEvent *ev)
{
    QPoint pos = ev->pos();
    if      ( colorRect().contains(pos) )  dragArea = DragArea::DragSV;
    else if ( hueRect().contains(pos) )    dragArea = DragArea::DragHue;
    else if ( alphaRect().contains(pos) )  dragArea = DragArea::DragAlpha;
    else                                   dragArea = DragArea::DragNone;
    // Now treat this press as a move, so the knob jumps immediately:
    mouseMoveEvent(ev);
}
void   ColorPickerWidget::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint pos = ev->pos();

    switch (dragArea)
    {
    case DragArea::DragSV: {
        auto r = colorRect();
        int cx = qBound(r.left(),   pos.x(), r.right());
        int cy = qBound(r.top(),    pos.y(), r.bottom());
        sat = float(cx - r.left())  / float(r.width()  - 1);
        val = 1.0f - float(cy - r.top()) / float(r.height() - 1);
        sat = qBound(0.0f, sat, 1.0f);
        val = qBound(0.0f, val, 1.0f);
        emit colorChanged(selectedColor());
        update();
        break;
    }
    case DragArea::DragHue: {
        auto r = hueRect();
        int hy = qBound(r.top(), pos.y(), r.bottom());
        hue = (1.0f - float(hy - r.top()) / float(r.height() - 1)) * 360.0f;
        hue = qBound(0.0f, hue, 360.0f);
        emit colorChanged(selectedColor());
        update();
        break;
    }
    case DragArea::DragAlpha: {
        auto r = alphaRect();
        int ay = qBound(r.top(), pos.y(), r.bottom());
        alpha = 1.0f - float(ay - r.top()) / float(r.height() - 1);
        alpha = qBound(0.0f, alpha, 1.0f);
        emit alphaChanged(alpha);
        emit colorChanged(selectedColor());
        update();
        break;
    }
    case DragArea::DragNone:
    default:
        // do nothing
        break;
    }
}
void   ColorPickerWidget::mouseReleaseEvent(QMouseEvent *)
{
    dragArea = DragArea::DragNone;
}
void   ColorPickerWidget::setColor(const QColor &color){
    double h, s, v;
    color.getHsvF(&h, &s, &v);
    hue = h * 360.0f;
    sat = s;
    val = v;
    alpha = color.alphaF();
    emit colorChanged(selectedColor());
    update();
}
void   ColorPickerWidget::wheelEvent(QWheelEvent *event){
    QPoint pos = event->position().toPoint();
    int delta = event->angleDelta().y();
    const int hueStep = 1;    // шаг по hue в градусах
    const float alphaStep = 0.01f; // шаг по alpha
    if (hueRect().contains(pos)) {
        hue += (delta > 0 ? hueStep : -hueStep);
        if (hue < 0.0f) hue += 360.0f;
        if (hue > 360.0f) hue -= 360.0f;
        hue = qBound(0.0f, hue, 360.0f);
        emit colorChanged(selectedColor());
        update();
        event->accept();
    } else if (alphaRect().contains(pos)) {
        alpha += (delta > 0 ? alphaStep : -alphaStep);
        alpha = qBound(0.0f, alpha, 1.0f);
        emit alphaChanged(alpha);
        emit colorChanged(selectedColor());
        update();
        event->accept();
    } else {
        event->ignore(); // если не над нужной областью
    }
}