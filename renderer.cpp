#include "renderer.h"
#include <QElapsedTimer>
#include <QThread>
#include <algorithm>

struct Renderer::Job {
    std::atomic_bool cancelled{false};
    std::atomic_int remaining{0};
    QImage image;
    View view;
    QElapsedTimer timer;
    quint64 id;
    bool preview;
};
Renderer::Renderer(QObject *parent) : QObject(parent) {
    pool.setMaxThreadCount(std::clamp(QThread::idealThreadCount() - 1, 1, 12));
    pool.setExpiryTimeout(-1);
}
Renderer::~Renderer() { cancel(); pool.waitForDone(); }
void Renderer::cancel() {
    ++generation;
    if (current) current->cancelled.store(true, std::memory_order_relaxed);
    pool.clear();
    current.reset();
}
quint64 Renderer::request(Fractal kind, View view, QSize size, bool preview) {
    cancel();
    auto job = std::make_shared<Job>();
    job->id = generation;
    job->view = view;
    job->preview = preview;
    job->image = QImage(size, QImage::Format_RGB32);
    current = job;
    if (job->image.isNull()) {
        QMetaObject::invokeMethod(this, [this, id=job->id] {
            if (id == generation) emit failed(id);
        }, Qt::QueuedConnection);
        return job->id;
    }
    auto *pixels = reinterpret_cast<QRgb *>(job->image.bits());
    const int stride = job->image.bytesPerLine()/sizeof(QRgb);
    const int tiles = std::min(size.height(), pool.maxThreadCount()*8);
    job->remaining = tiles;
    job->timer.start();
    for (int tile = 0; tile < tiles; ++tile) {
        const int begin = size.height()*tile/tiles;
        const int end = size.height()*(tile+1)/tiles;
        pool.start([this, job, pixels, stride, size, begin, end, kind] {
            renderRows(pixels, stride, size, begin, end, kind, job->view, job->cancelled);
            if (job->remaining.fetch_sub(1) == 1 && !job->cancelled.load()) {
                const qint64 elapsed = job->timer.elapsed();
                QMetaObject::invokeMethod(this, [this, job, elapsed] {
                    if (job->id == generation && !job->cancelled.load()) {
                        emit ready(job->id, job->image, job->view, job->preview, elapsed);
                        if (current == job) current.reset();
                    }
                }, Qt::QueuedConnection);
            }
        });
    }
    return job->id;
}
