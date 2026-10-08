#pragma once
#include "renderer.h"
#include <QElapsedTimer>
#include <QSet>
#include <QTimer>
#include <QWidget>

class Viewer : public QWidget {
    Q_OBJECT
public:
    explicit Viewer(QWidget *parent = nullptr);
    ~Viewer() override;
    void setScene(Fractal kind, const View &view);
    void setCzech(bool enabled);
    const View &view() const { return state; }
    Fractal fractal() const { return kind; }
    void zoom(double factor);
    quint64 renderRequests() const { return requests; }
signals:
    void viewChanged(View view);
    void renderFinished(bool preview, qint64 milliseconds);
    void framePainted();
protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    void focusOutEvent(QFocusEvent *) override;
private:
    void changed(bool navigation = true);
    void submit(bool preview);
    void keyboardStep();
    Renderer renderer;
    Fractal kind = Fractal::Mandelbrot;
    View state;
    QImage image;
    View imageView;
    QTimer previewTimer, refineTimer, keyTimer;
    QElapsedTimer keyClock;
    QSet<int> keys;
    QPointF lastMouse;
    bool dragging = false, czech = false, limit = false;
    enum class Status { Loading, Preview, Ready, Failed };
    Status status = Status::Loading;
    qint64 renderMs = 0;
    quint64 requests = 0;
};
