#include "window.h"
#include "lessons.h"
#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QScrollBar>
#include <QSignalSpy>
#include <QSpinBox>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTest>
#include <QTextBrowser>
#include <QWheelEvent>
#include <cmath>
#include <complex>

class Tests : public QObject {
    Q_OBJECT
private slots:
    void recurrence() {
        QCOMPARE(iterate(Fractal::Mandelbrot,{2,3},{.5,-.25}),QPointF(-4.5,11.75));
        QCOMPARE(iterate(Fractal::Julia,{2,3},{.5,-.25}),QPointF(-4.5,11.75));
        QCOMPARE(iterate(Fractal::BurningShip,{-2,3},{.5,-.25}),QPointF(-4.5,11.75));
        QCOMPARE(iterate(Fractal::Tricorn,{2,3},{.5,-.25}),QPointF(-4.5,-12.25));
    }
    void boundedAndEscaping() {
        for (int i=0;i<4;++i) {
            View v=defaultView(Fractal(i)); v.julia={0,0};
            QVERIFY(!sample(Fractal(i),{0,0},v).escaped);
            const Sample outside=sample(Fractal(i),{3,3},v);
            QVERIFY(outside.escaped); QVERIFY(std::isfinite(outside.smooth));
        }
        View v; v.julia={0,0};
        QVERIFY(!sample(Fractal::Julia,{.5,.5},v).escaped);
        QVERIFY(sample(Fractal::Julia,{1.1,0},v).escaped);
        // Compare the optimized loop with an independent complex-number reference.
        for (int k=0;k<4;++k) for (int ix=-8;ix<=6;++ix) for (int iy=-5;iy<=5;++iy) {
            const Fractal kind=Fractal(k); const View view=defaultView(kind);
            std::complex<double> p(ix*.23,iy*.21);
            std::complex<double> c=kind==Fractal::Julia?std::complex<double>(view.julia.x(),view.julia.y()):p;
            std::complex<double> z=kind==Fractal::Julia?p:0;
            const double radius=std::max(4.0,std::abs(c)+1); int n=0;
            while (std::abs(z)<=radius && n<view.iterations) {
                if (kind==Fractal::BurningShip) z={std::abs(z.real()),std::abs(z.imag())};
                if (kind==Fractal::Tricorn) z=std::conj(z);
                z=z*z+c; ++n;
            }
            QCOMPARE(sample(kind,{p.real(),p.imag()},view).escaped,std::abs(z)>radius);
        }
    }
    void coordinatesAndZoom() {
        for (int k=0;k<4;++k) {
            const Fractal kind=Fractal(k); View v=defaultView(kind); const QSize size(803,607);
            QCOMPARE(worldAt(v,size,{401.5,303.5},kind),v.center);
            const QPointF anchor(155.5,400.5),before=worldAt(v,size,anchor,kind);
            QVERIFY(zoomAt(v,size,anchor,.65,kind));
            QVERIFY(QLineF(before,worldAt(v,size,anchor,kind)).length()<1e-12);
            const QPointF p=worldAt(v,size,{200,200},kind);
            panBy(v,size,{40,-30},kind);
            QVERIFY(QLineF(p,worldAt(v,size,{240,170},kind)).length()<1e-12);
            QVERIFY(!zoomAt(v,size,anchor,0,kind));
            for (int j=0;j<100;++j) zoomAt(v,size,anchor,.5,kind);
            QVERIFY(v.span>0 && std::isfinite(v.span));
            QVERIFY(!zoomAt(v,size,anchor,.5,kind));
        }
    }
    void completePixelCoverage() {
        const QSize size(803,607); QImage image(size,QImage::Format_RGB32); image.fill(0);
        std::atomic_bool cancelled{false}; View v; v.iterations=20;
        for (int i=0;i<16;++i)
            renderRows(reinterpret_cast<QRgb*>(image.bits()),image.bytesPerLine()/4,size,
                       size.height()*i/16,size.height()*(i+1)/16,Fractal::Mandelbrot,v,cancelled);
        for (int y=0;y<size.height();++y) for (int x=0;x<size.width();++x)
            QVERIFY2(reinterpret_cast<const QRgb*>(image.constScanLine(y))[x]!=0,"Unrendered pixel");
        QCOMPARE(image.pixel(802,606),sampleColor(sample(Fractal::Mandelbrot,worldAt(v,size,{802.5,606.5},Fractal::Mandelbrot),v)));
        const QRgb a=sampleColor({true,10.01,10}),b=sampleColor({true,10.02,10});
        QVERIFY(std::abs(qRed(a)-qRed(b))<=1); QVERIFY(std::abs(qBlue(a)-qBlue(b))<=1);
    }
    void cancellationAndShutdown() {
        Renderer renderer; QSignalSpy spy(&renderer,&Renderer::ready);
        View expensive; expensive.iterations=5000; expensive.julia={0,0}; expensive.span=1;
        renderer.request(Fractal::Julia,expensive,{1600,900},false);
        QTest::qWait(10);
        for (int i=0;i<12;++i) renderer.request(Fractal(i%4),defaultView(Fractal(i%4)),{803+i,607},true);
        const quint64 id=renderer.request(Fractal::Tricorn,defaultView(Fractal::Tricorn),{321,243},false);
        QTRY_COMPARE_WITH_TIMEOUT(spy.count(),1,10000);
        QCOMPARE(spy[0][0].toULongLong(),id); QCOMPARE(qvariant_cast<QImage>(spy[0][1]).size(),QSize(321,243));
        QTest::qWait(100); QCOMPARE(spy.count(),1);
        QElapsedTimer timer; timer.start();
        { Renderer closing; closing.request(Fractal::Julia,expensive,{1600,900},false); QTest::qWait(10); }
        QVERIFY2(timer.elapsed()<1000,"Shutdown did not cancel promptly");
        QSignalSpy failed(&renderer,&Renderer::failed); renderer.request(Fractal::Julia,expensive,{},false);
        QTRY_COMPARE(failed.count(),1);
    }
    void interfaceAndTranslations() {
        QSettings().clear(); Window window; window.show();
        auto *viewer=window.canvas(); QSignalSpy finished(viewer,&Viewer::renderFinished);
        auto *list=window.findChild<QListWidget*>("fractalList");
        auto *tabs=window.findChild<QTabWidget*>("lessonTabs");
        auto *iterations=window.findChild<QSpinBox*>("iterations");
        auto *real=window.findChild<QDoubleSpinBox*>("juliaReal");
        auto *imag=window.findChild<QDoubleSpinBox*>("juliaImaginary");
        auto *language=window.findChild<QComboBox*>("language");
        auto *toggle=window.findChild<QPushButton*>("learnToggle");
        QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty() && !finished.last()[0].toBool(),10000);
        viewer->zoom(.7); const View saved=viewer->view(); list->setCurrentRow(1); list->setCurrentRow(0);
        QCOMPARE(viewer->view().span,saved.span); QCOMPARE(viewer->view().center,saved.center);
        language->setCurrentIndex(1); QVERIFY(window.isCzech()); QCOMPARE(viewer->view().span,saved.span);
        QCOMPARE(QSettings().value("language").toString(),QString("cs"));
        { Window restored; QVERIFY(restored.isCzech()); }
        QDir().mkpath("qa");
        for (int cs=0;cs<2;++cs) {
            language->setCurrentIndex(cs);
            for (int i=0;i<4;++i) {
                list->setCurrentRow(i);
                QCOMPARE(viewer->fractal(),Fractal(i));
                for (int tab=0;tab<3;++tab) {
                    tabs->setCurrentIndex(tab);
                    const QString content=window.findChild<QTextBrowser*>(QString("lesson%1").arg(tab))->toPlainText();
                    QVERIFY(content.size()>350);
                    QVERIFY(content.contains(cs?"Původní zdroje":"Read the sources"));
                    QVERIFY(!tabs->tabText(tab).isEmpty());
                }
                tabs->setCurrentIndex(0); finished.clear();
                QTest::mouseClick(window.findChild<QPushButton*>("tryButton"),Qt::LeftButton);
                QCOMPARE(viewer->view().center,guidedView(Fractal(i)).center);
                QCOMPARE(viewer->view().julia,guidedView(Fractal(i)).julia);
                QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty() && !finished.last()[0].toBool(),10000);
                QVERIFY(window.grab().save(QString("qa/%1-%2.png").arg(cs?"cs":"en").arg(i)));
                QTest::mouseClick(window.findChild<QPushButton*>("reset"),Qt::LeftButton);
                QCOMPARE(viewer->view().span,defaultView(Fractal(i)).span);
            }
        }
        list->setCurrentRow(1); real->setValue(-.4); imag->setValue(.6);
        QCOMPARE(viewer->view().julia,QPointF(-.4,.6));
        auto *presets=window.findChild<QComboBox*>("juliaPresets");
        presets->setCurrentIndex(2); emit presets->activated(2); QCOMPARE(viewer->view().julia,QPointF(0,1));
        iterations->setValue(900); QCOMPARE(viewer->view().iterations,900);
        list->setCurrentRow(0); list->setCurrentRow(1); QCOMPARE(viewer->view().iterations,900);
        QVERIFY(window.findChild<QWidget*>("learning")->isVisible()); QTest::mouseClick(toggle,Qt::LeftButton);
        QVERIFY(!window.findChild<QWidget*>("learning")->isVisible()); QTest::mouseClick(toggle,Qt::LeftButton);
        QVERIFY(window.findChild<QWidget*>("learning")->isVisible());
        // Rapid resize and selection must settle to the latest scene.
        for (int i=0;i<12;++i) { window.resize(1100+i*7,720+i); list->setCurrentRow(i%4); }
        finished.clear(); list->setCurrentRow(0); window.resize(1400,860);
        QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty() && !finished.last()[0].toBool(),10000);
        const quint64 requests=viewer->renderRequests(); QTest::qWait(350); QCOMPARE(viewer->renderRequests(),requests);
        finished.clear(); window.resize(1000,620);
        QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty() && !finished.last()[0].toBool(),10000);
        QCOMPARE(list->horizontalScrollBar()->isVisible(),false);
        QVERIFY(window.grab().save("qa/cs-small.png"));
        finished.clear(); list->setCurrentRow(1);
        QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty() && !finished.last()[0].toBool(),10000);
        QVERIFY(window.grab().save("qa/cs-small-julia.png"));
    }
    void navigationAndLatency() {
        Viewer viewer; viewer.resize(800,600); viewer.show(); viewer.setFocus();
        QSignalSpy finished(&viewer,&Viewer::renderFinished),painted(&viewer,&Viewer::framePainted);
        QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty() && !finished.last()[0].toBool(),10000);
        const QPointF anchor(200,180); const QPointF fixed=worldAt(viewer.view(),viewer.size(),anchor,viewer.fractal());
        QWheelEvent wheel(anchor,viewer.mapToGlobal(anchor.toPoint()),{},QPoint(0,120),Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);
        painted.clear(); QElapsedTimer latency; latency.start(); QApplication::sendEvent(&viewer,&wheel);
        while (painted.isEmpty() && latency.elapsed()<200) QApplication::processEvents();
        QVERIFY(!painted.isEmpty()); qInfo()<<"Navigation-to-paint:"<<latency.elapsed()<<"ms";
        QVERIFY2(latency.elapsed()<50,"Navigation paint exceeded 50 ms target");
        QVERIFY(QLineF(fixed,worldAt(viewer.view(),viewer.size(),anchor,viewer.fractal())).length()<1e-12);
        const QPointF before=viewer.view().center;
        QTest::mousePress(&viewer,Qt::LeftButton,Qt::NoModifier,{400,300});
        QTest::mouseMove(&viewer,{450,330}); QTest::mouseRelease(&viewer,Qt::LeftButton,Qt::NoModifier,{450,330});
        QVERIFY(viewer.view().center!=before);
        const double span=viewer.view().span;
        QTest::keyPress(&viewer,Qt::Key_P); QTest::qWait(70); QTest::keyRelease(&viewer,Qt::Key_P);
        QVERIFY(viewer.view().span<span);
        const QPointF center=viewer.view().center;
        QTest::keyPress(&viewer,Qt::Key_Right); QTest::qWait(50); QTest::keyRelease(&viewer,Qt::Key_Right);
        QVERIFY(viewer.view().center.x()>center.x());
        // Losing focus must stop held-key navigation.
        QTest::keyPress(&viewer,Qt::Key_Left);
        QFocusEvent focusOut(QEvent::FocusOut); QApplication::sendEvent(&viewer,&focusOut);
        const QPointF stopped=viewer.view().center; QTest::qWait(80); QCOMPARE(viewer.view().center,stopped);
    }
};
int main(int argc,char **argv) {
    QApplication app(argc,argv); app.setStyle("Fusion");
    app.setOrganizationName("KomplexTests"); app.setApplicationName("IsolatedTests");
    QTemporaryDir settings;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,settings.path());
    Tests tests; return QTest::qExec(&tests,argc,argv);
}
#include "tests.moc"
