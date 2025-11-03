//FILE: QtPallete/qtpalette.h
#ifndef COLORPICKERWIDGET_H // Include guard — prevents this header file from
#define COLORPICKERWIDGET_H // being included more than once during compilation.

#include <QWidget> // Include the base QWidget class 
#include <QColor>  // Include QColor for color handling.

enum class ColorMode { ARGB, CMYK, HSV, HSL };
static const std::array<ColorMode,4> ModeOrder = {
    ColorMode::ARGB,
    ColorMode::CMYK,
    ColorMode::HSV,
    ColorMode::HSL
};
extern ColorMode currentColorMode;
class ColorPickerWidget : public QWidget 
{
    Q_OBJECT // Defines a custom widget based on QWidget
public:      // Required macro for using Qt's signal-slot mechanism.
    explicit ColorPickerWidget(QWidget *parent = nullptr); // Constructor with optional parent widget.
    QColor   selectedColor() const;                          // Returns the currently selected color.
    float    selectedAlpha() const { return alpha; }
public slots:
    void setColor(const QColor &color);
signals:     // Signal that gets emitted when the color changes.      
    void colorChanged(const QColor &color);
    void alphaChanged(float alpha);
protected:   // Reimplemented Qt event handlers for painting and mouse interaction.
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent *) override;
private:     // Helper functions and HSV color data stored internally.
    enum class DragArea { DragNone, DragSV, DragHue, DragAlpha };
    DragArea dragArea = DragArea::DragNone;
    QRect  colorRect() const;
    QRect  alphaRect() const;
    QRect  hueRect()   const;
    QColor hsvToColor(float h, float s, float v);

    float hue = 0.0f;     // 0..360 inialize hue
    float sat = 1.0f;     // 0..1   initialize saturation
    float val = 1.0f;     // 0..1
    float alpha = 1.0f;   // 0..1   alpha
};

#endif // COLORPICKERWIDGET_H
