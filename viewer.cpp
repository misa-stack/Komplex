#include "viewer.h"
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

Viewer::Viewer(QWidget *parent) : QWidget(parent) {
    setObjectName("viewer");
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(320, 300);
    setCursor(Qt::OpenHandCursor);
    setAttribute(Qt::WA_OpaquePaintEvent);
    previewTimer.setSingleShot(true);
    previewTimer.setInterval(32);
    refineTimer.setSingleShot(true);
    refineTimer.setInterval(180);
    keyTimer.setInterval(16);
    connect(&previewTimer, &QTimer::timeout, this, [this] { submit(true); });
    connect(&refineTimer, &QTimer::timeout, this, [this] { previewTimer.stop(); submit(false); });
    connect(&keyTimer, &QTimer::timeout, this, &Viewer::keyboardStep);
    connect(&renderer, &Renderer::ready, this, [this](quint64, QImage result, View v, bool preview, qint64 ms) {
        image = std::move(result);
        imageView = v;
        status = preview ? Status::Preview : Status::Ready;
        renderMs = ms;
        update();
        emit renderFinished(preview, ms);
    });
    connect(&renderer, &Renderer::failed, this, [this] { status = Status::Failed; update(); });
    setCzech(false);
}
Viewer::~Viewer() { renderer.cancel(); }
void Viewer::setCzech(bool enabled) {
    czech = enabled;
    setAccessibleName(czech ? "Interaktivní zobrazení fraktálu" : "Interactive fractal viewer");
    setToolTip(czech ? "Tažením posunete obraz. Kolečkem přiblížíte. Šipky: posun; P/O: přiblížení/oddálení."
                     : "Drag to pan. Scroll to zoom. Arrow keys: pan; P/O: zoom in/out.");
    update();
}
void Viewer::setScene(Fractal next, const View &v) {
    const bool incompatible = next != kind || v.julia != state.julia;
    kind = next;
    state = v;
    if (incompatible) image = QImage();
    keys.clear(); keyTimer.stop(); dragging = false; setCursor(Qt::OpenHandCursor);
    limit = false;
    changed(false);
}
void Viewer::changed(bool navigation) {
    renderer.cancel();
    status = Status::Loading;
    if (!previewTimer.isActive()) previewTimer.start();
    refineTimer.start();
    update();
    if (navigation) emit viewChanged(state);
}
void Viewer::submit(bool preview) {
    if (width() <= 0 || height() <= 0) return;
    const double scale = preview ? .33 : devicePixelRatioF();
    const QSize pixels(std::max(1, int(width()*scale)), std::max(1, int(height()*scale)));
    View requestView = state;
    if (preview) requestView.iterations = std::min(state.iterations, 180);
    ++requests;
    renderer.request(kind, requestView, pixels, preview);
}
void Viewer::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#080e17"));
    if (!image.isNull()) {
        const double scale = width()/state.span;
        const double targetWidth = imageView.span*scale;
        const double targetHeight = targetWidth*image.height()/image.width();
        const QPointF center(width()/2.0 + (imageView.center.x()-state.center.x())*scale,
                             height()/2.0 + (imageView.center.y()-state.center.y())*scale*yDirection(kind));
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawImage(QRectF(center.x()-targetWidth/2, center.y()-targetHeight/2,
                                  targetWidth, targetHeight), image);
    }
    QString label;
    if (limit) label = czech ? "Dosažen limit přiblížení" : "Zoom limit reached";
    else if (status == Status::Loading) label = czech ? "Vykreslování…" : "Rendering…";
    else if (status == Status::Preview) label = czech ? "Náhled · doplňování detailů…" : "Preview · refining detail…";
    else if (status == Status::Failed) label = czech ? "Nedostatek paměti. Zmenšete okno." : "Not enough memory. Resize the window.";
    else label = QString(czech ? "Hotovo · %1 ms" : "Ready · %1 ms").arg(renderMs);
    const QString magnification = QString("%1×").arg(defaultView(kind).span/state.span, 0, 'g', 5);
    const auto pill = [&painter](const QString &text, QPoint position) {
        const QRect box(position, QSize(painter.fontMetrics().horizontalAdvance(text)+24, 30));
        painter.setPen(Qt::NoPen); painter.setBrush(QColor(15,25,37,225));
        painter.drawRoundedRect(box, 6, 6);
        painter.setPen(QColor("#bfd3db")); painter.drawText(box, Qt::AlignCenter, text);
    };
    pill(label, {16, height()-46});
    pill(magnification, {16,16});
    if (hasFocus()) {
        painter.setBrush(Qt::NoBrush); painter.setPen(QPen(QColor("#5eaaa6"), 1));
        painter.drawRect(rect().adjusted(0,0,-1,-1));
    }
    emit framePainted();
}
void Viewer::resizeEvent(QResizeEvent *) { changed(false); }
void Viewer::zoom(double factor) {
    limit = !zoomAt(state, size(), {width()/2.0,height()/2.0}, factor, kind);
    changed();
}
void Viewer::wheelEvent(QWheelEvent *event) {
    const double steps = event->pixelDelta().isNull() ? event->angleDelta().y()/120.0 : event->pixelDelta().y()/80.0;
    limit = !zoomAt(state, size(), event->position(), std::exp(-std::clamp(steps,-10.0,10.0)*.18), kind);
    changed(); event->accept();
}
void Viewer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        setFocus(); dragging = true; lastMouse = event->position(); setCursor(Qt::ClosedHandCursor); event->accept();
    } else QWidget::mousePressEvent(event);
}
void Viewer::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        panBy(state, size(), event->position()-lastMouse, kind);
        lastMouse = event->position(); limit = false; changed();
    }
}
void Viewer::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) { dragging = false; setCursor(Qt::OpenHandCursor); }
}
static bool navigationKey(int key) {
    return key == Qt::Key_Left || key == Qt::Key_Right || key == Qt::Key_Up || key == Qt::Key_Down
           || key == Qt::Key_P || key == Qt::Key_O;
}
void Viewer::keyPressEvent(QKeyEvent *event) {
    if (!navigationKey(event->key())) { QWidget::keyPressEvent(event); return; }
    if (!event->isAutoRepeat()) {
        keys.insert(event->key());
        if (!keyTimer.isActive()) { keyClock.start(); keyTimer.start(); keyboardStep(); }
    }
    event->accept();
}
void Viewer::keyReleaseEvent(QKeyEvent *event) {
    if (!navigationKey(event->key())) { QWidget::keyReleaseEvent(event); return; }
    if (!event->isAutoRepeat()) { keys.remove(event->key()); if (keys.isEmpty()) keyTimer.stop(); }
    event->accept();
}
void Viewer::keyboardStep() {
    const double dt = std::clamp(keyClock.restart()/1000.0, .016, .05);
    const int dx = keys.contains(Qt::Key_Left)-keys.contains(Qt::Key_Right);
    const int dy = keys.contains(Qt::Key_Up)-keys.contains(Qt::Key_Down);
    panBy(state, size(), QPointF(dx,dy)*280*dt, kind);
    const int direction = keys.contains(Qt::Key_O)-keys.contains(Qt::Key_P);
    limit = direction && !zoomAt(state, size(), {width()/2.0,height()/2.0}, std::exp(direction*1.6*dt), kind);
    changed();
}
void Viewer::focusOutEvent(QFocusEvent *event) {
    keys.clear(); keyTimer.stop(); dragging = false; setCursor(Qt::OpenHandCursor);
    QWidget::focusOutEvent(event); update();
}
