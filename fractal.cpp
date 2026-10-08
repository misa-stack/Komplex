#include "fractal.h"
#include <algorithm>
#include <cmath>
#include <limits>

View defaultView(Fractal kind) {
    View v;
    if (kind == Fractal::Julia) { v.center = {0, 0}; v.span = 3.5; }
    if (kind == Fractal::BurningShip) { v.center = {-0.4, -0.4}; v.span = 4.2; }
    if (kind == Fractal::Tricorn) { v.center = {-0.3, 0}; v.span = 4.2; }
    return v;
}
View guidedView(Fractal kind) {
    View v = defaultView(kind);
    switch (kind) {
    case Fractal::Mandelbrot: v.center = {-0.745, 0.18}; v.span = 0.15; break;
    case Fractal::Julia: v.julia = {-1, 0}; break;
    case Fractal::BurningShip: v.center = {-1.76, -0.035}; v.span = 0.12; break;
    case Fractal::Tricorn: v.center = {-1.5, 0}; v.span = 0.85; break;
    }
    return v;
}
QString fractalName(Fractal kind, bool cs) {
    switch (kind) {
    case Fractal::Mandelbrot: return cs ? "Mandelbrotova množina" : "Mandelbrot";
    case Fractal::Julia: return cs ? "Juliova množina" : "Julia";
    case Fractal::BurningShip: return cs ? "Hořící loď" : "Burning Ship";
    case Fractal::Tricorn: return cs ? "Trojrožec" : "Tricorn";
    }
    return {};
}
QString formula(Fractal kind) {
    if (kind == Fractal::BurningShip) return "zₙ₊₁ = (|Re zₙ| + i |Im zₙ|)² + c";
    if (kind == Fractal::Tricorn) return "zₙ₊₁ = z̄ₙ² + c";
    return "zₙ₊₁ = zₙ² + c";
}
double yDirection(Fractal kind) { return kind == Fractal::BurningShip ? 1.0 : -1.0; }
QPointF worldAt(const View &v, QSize size, QPointF p, Fractal kind) {
    const double scale = v.span / std::max(1, size.width());
    return v.center + QPointF((p.x() - size.width() / 2.0) * scale,
                             (p.y() - size.height() / 2.0) * scale * yDirection(kind));
}
bool zoomAt(View &v, QSize size, QPointF anchor, double factor, Fractal kind) {
    if (!std::isfinite(factor) || factor <= 0) return false;
    const double minSpan = 64 * std::numeric_limits<double>::epsilon()
        * std::max({1.0, std::abs(v.center.x()), std::abs(v.center.y())}) * std::max(1, size.width());
    const double requested = v.span * factor;
    const double span = std::clamp(requested, minSpan, 12.0);
    const auto fixed = worldAt(v, size, anchor, kind);
    v.span = span;
    v.center += fixed - worldAt(v, size, anchor, kind);
    return span == requested;
}
void panBy(View &v, QSize size, QPointF delta, Fractal kind) {
    const double scale = v.span / std::max(1, size.width());
    v.center -= QPointF(delta.x() * scale, delta.y() * scale * yDirection(kind));
    v.center.setX(std::clamp(v.center.x(), -10.0, 10.0));
    v.center.setY(std::clamp(v.center.y(), -10.0, 10.0));
}
QPointF iterate(Fractal kind, QPointF z, QPointF c) {
    double x = z.x(), y = z.y();
    if (kind == Fractal::BurningShip) { x = std::abs(x); y = std::abs(y); }
    if (kind == Fractal::Tricorn) y = -y;
    return {x*x - y*y + c.x(), 2*x*y + c.y()};
}
Sample sample(Fractal kind, QPointF point, const View &v, const std::atomic_bool *cancel) {
    double x = kind == Fractal::Julia ? point.x() : 0;
    double y = kind == Fractal::Julia ? point.y() : 0;
    const double cx = kind == Fractal::Julia ? v.julia.x() : point.x();
    const double cy = kind == Fractal::Julia ? v.julia.y() : point.y();
    // Analytic interior checks apply only to the Mandelbrot family.
    if (kind == Fractal::Mandelbrot) {
        const double q = (cx-.25)*(cx-.25) + cy*cy;
        if (q*(q+cx-.25) <= .25*cy*cy || (cx+1)*(cx+1)+cy*cy <= .0625)
            return {false, 0, v.iterations};
    }
    const double radius = std::max(4.0, std::hypot(cx, cy) + 1.0);
    const double escape2 = radius * radius;
    int n = 0;
    while (x*x+y*y <= escape2 && n < v.iterations) {
        if ((n & 63) == 0 && cancel && cancel->load(std::memory_order_relaxed))
            return {false, 0, n};
        if (kind == Fractal::BurningShip) { x = std::abs(x); y = std::abs(y); }
        if (kind == Fractal::Tricorn) y = -y;
        const double nextX = x*x-y*y+cx;
        y = 2*x*y+cy;
        x = nextX;
        ++n;
    }
    if (x*x+y*y <= escape2) return {false, 0, n};
    const double smooth = n + 1 - std::log2(std::log(std::hypot(x,y)));
    return {true, std::max(0.0, smooth), n};
}
QRgb sampleColor(const Sample &s) {
    if (!s.escaped) return qRgb(8, 14, 23);
    const double stops[][3] = {{12,24,48},{24,64,105},{39,126,156},{104,202,193},
                              {232,235,190},{234,167,94},{155,67,64},{12,24,48}};
    const double t = std::fmod(std::sqrt(s.smooth) * .8, 7.0);
    const int i = int(t);
    const double f = t - i;
    return qRgb(int(stops[i][0]*(1-f)+stops[i+1][0]*f),
                int(stops[i][1]*(1-f)+stops[i+1][1]*f),
                int(stops[i][2]*(1-f)+stops[i+1][2]*f));
}
void renderRows(QRgb *pixels, int stride, QSize size, int begin, int end,
                Fractal kind, const View &view, const std::atomic_bool &cancel) {
    for (int y = begin; y < end; ++y) {
        if (cancel.load(std::memory_order_relaxed)) return;
        for (int x = 0; x < size.width(); ++x) {
            if ((x & 31) == 0 && cancel.load(std::memory_order_relaxed)) return;
            pixels[y*stride+x] = sampleColor(sample(kind, worldAt(view, size, {x+.5,y+.5}, kind), view, &cancel));
        }
    }
}
