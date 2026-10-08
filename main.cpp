#include "window.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include <QTimer>

int main(int argc,char **argv) {
    QApplication app(argc,argv);
    app.setOrganizationName("Komplex"); app.setApplicationName("Komplex"); app.setApplicationVersion("2.0");
    app.setStyle("Fusion");
    QCommandLineParser parser;
    parser.setApplicationDescription("Komplex — bilingual fractal explorer");
    parser.addHelpOption(); parser.addVersionOption();
    parser.addOption({"benchmark","Measure full-detail rendering at two resolutions."});
    parser.addOption({"screenshot","Save a rendered window screenshot and exit.","path"});
    parser.addOption({"language","Start in en or cs (and remember the choice).","language"});
    parser.process(app);
    if (parser.isSet("language")) {
        const QString language=parser.value("language");
        if (language!="en" && language!="cs") { QTextStream(stderr)<<"Language must be en or cs.\n"; return 2; }
        QSettings().setValue("language",language);
    }
    if (parser.isSet("benchmark")) {
        Renderer renderer;
        QTextStream output(stdout);
        output<<"Workers: "<<renderer.workerCount()<<"\nfractal,width,height,iterations,milliseconds\n";
        for (const QSize size : {QSize(800,600),QSize(1600,900)}) for (int i=0;i<4;++i) {
            QEventLoop loop; qint64 duration=-1;
            auto done=QObject::connect(&renderer,&Renderer::ready,&loop,[&](quint64,QImage,View,bool,qint64 ms){duration=ms;loop.quit();});
            auto fail=QObject::connect(&renderer,&Renderer::failed,&loop,&QEventLoop::quit);
            QTimer timeout; timeout.setSingleShot(true); QObject::connect(&timeout,&QTimer::timeout,&loop,&QEventLoop::quit); timeout.start(60000);
            renderer.request(Fractal(i),defaultView(Fractal(i)),size,false); loop.exec();
            QObject::disconnect(done); QObject::disconnect(fail);
            if (duration<0) { output<<"Render failed or timed out\n"; return 1; }
            output<<fractalName(Fractal(i),false)<<","<<size.width()<<","<<size.height()<<",600,"<<duration<<"\n"; output.flush();
        }
        return 0;
    }
    Window window; window.show();
    if (parser.isSet("screenshot")) {
        const QString path=parser.value("screenshot");
        auto captured=std::make_shared<bool>(false);
        QObject::connect(window.canvas(),&Viewer::renderFinished,&window,[&,path,captured](bool preview,qint64){
            if (preview || *captured) return;
            *captured=true;
            QTimer::singleShot(100,&window,[&,path]{app.exit(window.grab().save(path)?0:1);});
        });
        QTimer::singleShot(30000,&app,[&]{app.exit(1);});
    }
    return app.exec();
}
