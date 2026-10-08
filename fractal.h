#pragma once
#include <QPointF>
#include <QSize>
#include <QString>
#include <QRgb>
#include <atomic>

enum class Fractal { Mandelbrot, Julia, BurningShip, Tricorn };
struct View {
    QPointF center{-0.5, 0.0};
    double span = 3.6;
    int iterations = 600;
    QPointF julia{-0.8, 0.156};
};
struct Sample { bool escaped; double smooth; int iterations; };
View defaultView(Fractal kind);
View guidedView(Fractal kind);
QString fractalName(Fractal kind, bool czech);
QString formula(Fractal kind);
double yDirection(Fractal kind);
QPointF worldAt(const View &view, QSize size, QPointF pixel, Fractal kind);
bool zoomAt(View &view, QSize size, QPointF anchor, double factor, Fractal kind);
void panBy(View &view, QSize size, QPointF delta, Fractal kind);
QPointF iterate(Fractal kind, QPointF z, QPointF c);
Sample sample(Fractal kind, QPointF point, const View &view,
              const std::atomic_bool *cancel = nullptr);
QRgb sampleColor(const Sample &value);
void renderRows(QRgb *pixels, int stride, QSize size, int begin, int end,
                Fractal kind, const View &view, const std::atomic_bool &cancel);
