#pragma once
#include "fractal.h"
#include <QImage>
#include <QObject>
#include <QThreadPool>
#include <memory>

class Renderer : public QObject {
    Q_OBJECT
public:
    explicit Renderer(QObject *parent = nullptr);
    ~Renderer() override;
    quint64 request(Fractal kind, View view, QSize size, bool preview);
    void cancel();
    int workerCount() const { return pool.maxThreadCount(); }
signals:
    void ready(quint64 id, QImage image, View view, bool preview, qint64 milliseconds);
    void failed(quint64 id);
private:
    struct Job;
    QThreadPool pool;
    std::shared_ptr<Job> current;
    quint64 generation = 0;
};
Q_DECLARE_METATYPE(View)
