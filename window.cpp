#include "window.h"
#include "lessons.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QSplitter>
#include <QTabWidget>
#include <QTabBar>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QScrollArea>

namespace {
QLabel *label(const QString &name, QWidget *parent = nullptr) {
    auto *result = new QLabel(parent); result->setObjectName(name); return result;
}
QPushButton *button(const QString &name) {
    auto *result = new QPushButton; result->setObjectName(name); result->setCursor(Qt::PointingHandCursor); return result;
}
}
Window::Window(QWidget *parent) : QMainWindow(parent) {
    setObjectName("komplex"); resize(1400,860); setMinimumSize(1000,620);
    for (int i=0; i<4; ++i) states[i]=defaultView(Fractal(i));
    czech = QSettings().value("language", "en").toString() == "cs";
    auto *root = new QWidget; setCentralWidget(root);
    auto *layout = new QVBoxLayout(root); layout->setContentsMargins(0,0,0,0); layout->setSpacing(0);
    auto *top = new QWidget; top->setObjectName("topBar");
    auto *topLayout = new QHBoxLayout(top); topLayout->setContentsMargins(24,17,24,17);
    auto *brand = new QLabel("K O M P L E X"); brand->setObjectName("brand"); topLayout->addWidget(brand);
    subtitle=label("subtitle"); topLayout->addSpacing(18); topLayout->addWidget(subtitle); topLayout->addStretch();
    learnToggle=button("learnToggle"); learnToggle->setCheckable(true); learnToggle->setChecked(true); topLayout->addWidget(learnToggle);
    language = new QComboBox; language->setObjectName("language"); language->addItems({"English", "Čeština"});
    language->setCurrentIndex(czech ? 1 : 0); topLayout->addSpacing(12); topLayout->addWidget(language);
    layout->addWidget(top);
    auto *body = new QHBoxLayout; body->setContentsMargins(0,0,0,0); body->setSpacing(0); layout->addLayout(body,1);
    auto *sidebar = new QWidget; sidebar->setObjectName("sidebar");
    auto *side = new QVBoxLayout(sidebar); side->setContentsMargins(18,24,18,20); side->setSpacing(14);
    side->setSizeConstraint(QLayout::SetMinimumSize);
    collectionLabel=label("eyebrow"); side->addWidget(collectionLabel);
    fractalList=new QListWidget; fractalList->setObjectName("fractalList"); fractalList->setFixedHeight(212);
    fractalList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    for (int i=0;i<4;++i) { auto *item=new QListWidgetItem(fractalList); item->setSizeHint({0,49}); }
    fractalList->setCurrentRow(0); side->addWidget(fractalList);
    controlsLabel=label("eyebrow"); side->addWidget(controlsLabel);
    iterationsLabel=label("fieldLabel"); iterations = new QSpinBox; iterations->setObjectName("iterations");
    iterations->setRange(100,5000); iterations->setSingleStep(100); iterations->setKeyboardTracking(false);
    iterationsLabel->setBuddy(iterations); side->addWidget(iterationsLabel); side->addWidget(iterations);
    juliaControls=new QWidget; auto *juliaLayout=new QVBoxLayout(juliaControls); juliaLayout->setContentsMargins(0,0,0,0);
    juliaLabel=label("fieldLabel"); presets=new QComboBox; presets->setObjectName("juliaPresets");
    juliaLayout->addWidget(juliaLabel); juliaLayout->addWidget(presets);
    realPart=new QDoubleSpinBox; imagPart=new QDoubleSpinBox;
    realPart->setObjectName("juliaReal"); imagPart->setObjectName("juliaImaginary");
    for (auto *spin : {realPart,imagPart}) { spin->setRange(-2,2); spin->setDecimals(6); spin->setSingleStep(.005); spin->setKeyboardTracking(false); }
    auto *form=new QFormLayout; realLabel=label("fieldLabel"); imagLabel=label("fieldLabel");
    realLabel->setBuddy(realPart); imagLabel->setBuddy(imagPart); form->addRow(realLabel,realPart); form->addRow(imagLabel,imagPart);
    juliaLayout->addLayout(form); side->addWidget(juliaControls);
    reset=button("reset"); side->addWidget(reset); side->addStretch(1);
    hint=label("hint"); hint->setWordWrap(true); side->addWidget(hint);
    auto *sideScroll=new QScrollArea; sideScroll->setObjectName("sideScroll");
    sideScroll->setFixedWidth(230); sideScroll->setFrameShape(QFrame::NoFrame);
    sideScroll->setWidgetResizable(true); sideScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sideScroll->setWidget(sidebar); body->addWidget(sideScroll);
    splitter=new QSplitter; splitter->setObjectName("mainSplitter"); splitter->setChildrenCollapsible(false);
    body->addWidget(splitter,1);
    auto *explorer=new QWidget; auto *explorerLayout=new QVBoxLayout(explorer); explorerLayout->setContentsMargins(0,0,0,0); explorerLayout->setSpacing(0);
    auto *viewerHeader=new QWidget; viewerHeader->setObjectName("viewerHeader"); auto *headerLayout=new QHBoxLayout(viewerHeader);
    headerLayout->setContentsMargins(22,18,18,18); auto *heading=new QVBoxLayout;
    title=label("fractalTitle"); title->setWordWrap(true);
    equation=label("equation"); equation->setWordWrap(true); heading->addWidget(title); heading->addWidget(equation); headerLayout->addLayout(heading,1);
    zoomOut=button("zoomOut"); zoomOut->setText("−"); zoomOut->setFixedWidth(38);
    zoomIn=button("zoomIn"); zoomIn->setText("+"); zoomIn->setFixedWidth(38);
    headerLayout->addWidget(zoomOut); headerLayout->addWidget(zoomIn); explorerLayout->addWidget(viewerHeader);
    viewer=new Viewer; explorerLayout->addWidget(viewer,1); splitter->addWidget(explorer);
    learning=new QWidget; learning->setObjectName("learning"); learning->setMinimumWidth(355);
    auto *learnLayout=new QVBoxLayout(learning); learnLayout->setContentsMargins(22,24,22,20); learnLayout->setSpacing(16);
    lessonLabel=label("eyebrow"); learnLayout->addWidget(lessonLabel);
    tabs=new QTabWidget; tabs->setObjectName("lessonTabs"); tabs->setDocumentMode(true);
    tabs->tabBar()->setDrawBase(false);
    for (int i=0;i<3;++i) {
        pages[i]=new QTextBrowser; pages[i]->setObjectName(QString("lesson%1").arg(i)); pages[i]->setOpenExternalLinks(true);
        pages[i]->setFrameShape(QFrame::NoFrame); pages[i]->document()->setDocumentMargin(2);
        tabs->addTab(pages[i],QString());
    }
    learnLayout->addWidget(tabs,1);
    tryDescription=label("tryDescription"); tryDescription->setWordWrap(true); learnLayout->addWidget(tryDescription);
    tryButton=button("tryButton"); learnLayout->addWidget(tryButton);
    splitter->addWidget(learning); splitter->setStretchFactor(0,1); splitter->setStretchFactor(1,0); splitter->setSizes({760,410});

    connect(language,&QComboBox::currentIndexChanged,this,[this](int index){setCzech(index==1);});
    connect(fractalList,&QListWidget::currentRowChanged,this,&Window::selectFractal);
    connect(viewer,&Viewer::viewChanged,this,[this](View v){states[int(selected)]=v;});
    connect(iterations,&QSpinBox::valueChanged,this,[this](int value){states[int(selected)].iterations=value; viewer->setScene(selected,states[int(selected)]);});
    connect(realPart,&QDoubleSpinBox::valueChanged,this,[this]{parametersChanged();});
    connect(imagPart,&QDoubleSpinBox::valueChanged,this,[this]{parametersChanged();});
    connect(presets,&QComboBox::activated,this,[this](int index){
        if (index == 3) return;
        const QPointF values[]={{-.8,.156},{-1,0},{0,1}};
        states[1].julia=values[index]; viewer->setScene(selected,states[1]); refreshControls();
    });
    connect(reset,&QPushButton::clicked,this,[this]{states[int(selected)]=defaultView(selected); refreshControls(); viewer->setScene(selected,states[int(selected)]); viewer->setFocus();});
    connect(tryButton,&QPushButton::clicked,this,[this]{states[int(selected)]=guidedView(selected); refreshControls(); viewer->setScene(selected,states[int(selected)]); viewer->setFocus();});
    connect(zoomIn,&QPushButton::clicked,this,[this]{viewer->zoom(1/1.3);});
    connect(zoomOut,&QPushButton::clicked,this,[this]{viewer->zoom(1.3);});
    connect(learnToggle,&QPushButton::toggled,learning,&QWidget::setVisible);
    setStyleSheet(R"(
        QMainWindow, QWidget { background:#121e2b; color:#d8e3e8; font-family:"DejaVu Sans"; font-size:13px; }
        QWidget#topBar { background:#0d1722; border-bottom:1px solid #293746; }
        QLabel#brand { font-size:18px; font-weight:600; color:#f0f1e7; background:transparent; }
        QLabel#subtitle { color:#9dafba; background:transparent; }
        QWidget#sidebar { background:#101b27; border-right:1px solid #293746; }
        QLabel { background:transparent; }
        QLabel#eyebrow { color:#7eb9b1; font-size:11px; font-weight:600; letter-spacing:1px; }
        QLabel#fractalTitle { color:#f2f4ee; font-size:23px; font-weight:600; }
        QLabel#equation { color:#9aafb9; font-size:12px; }
        QLabel#hint { color:#91a6b5; font-size:12px; line-height:150%; }
        QLabel#fieldLabel { color:#a9bcc5; }
        QLabel#tryDescription { color:#b7cbd1; font-size:13px; }
        QListWidget { background:transparent; border:0; outline:0; }
        QListWidget::item { border-radius:6px; padding-left:8px; margin-bottom:3px; }
        QListWidget::item:selected { background:#25433f; color:#a9edd8; }
        QListWidget::item:hover:!selected { background:#1c2d3c; }
        QPushButton { background:#203241; border:1px solid #3a5060; border-radius:5px; padding:9px 12px; }
        QPushButton:hover { background:#2b4352; border-color:#7eb9b1; }
        QPushButton:pressed { background:#385768; }
        QPushButton:checked { background:#24443f; color:#a9edd8; border-color:#49766b; }
        QPushButton#tryButton { background:#83cdb6; color:#0a2925; border:0; font-weight:600; padding:12px; }
        QPushButton#tryButton:hover { background:#a3e3ce; }
        QComboBox, QSpinBox, QDoubleSpinBox { background:#0d1722; border:1px solid #3a5060; border-radius:4px; padding:7px; min-height:19px; }
        QComboBox QAbstractItemView { background:#203241; selection-background-color:#355a56; }
        QTabWidget::pane { border:0; }
        QTabBar::base { border:0; background:transparent; }
        QTabBar::tab { background:transparent; color:#a2b5c1; border-bottom:2px solid #2b3c4c; padding:10px 9px; }
        QTabBar::tab:selected { color:#a9edd8; border-color:#83cdb6; }
        QTextBrowser { background:transparent; border:0; selection-background-color:#365e5b; }
        QScrollBar:vertical { background:#121e2b; width:7px; margin:0; }
        QScrollBar::handle:vertical { background:#405c6c; min-height:28px; border-radius:3px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
        QSplitter::handle { background:#293746; width:1px; }
        QPushButton:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QListWidget:focus { border:1px solid #a9edd8; }
        QToolTip { color:#edf4ed; background:#243d4b; border:1px solid #537b89; padding:5px; }
    )");
    refreshTexts(); refreshControls(); viewer->setScene(selected,states[0]); viewer->setCzech(czech);
}
QString Window::text(const char *en,const char *cs) const { return QString::fromUtf8(czech?cs:en); }
void Window::setCzech(bool enabled) {
    czech=enabled; QSettings().setValue("language",czech?"cs":"en");
    const QSignalBlocker block(language); language->setCurrentIndex(czech?1:0);
    refreshTexts(); refreshControls(); viewer->setCzech(czech);
}
void Window::selectFractal(int index) {
    if (index<0 || index>3) return;
    selected=Fractal(index); refreshTexts(); refreshControls(); viewer->setScene(selected,states[index]);
}
void Window::refreshTexts() {
    setWindowTitle(text("Komplex · Fractal explorer","Komplex · Průzkumník fraktálů"));
    subtitle->setText(text("A field guide to infinity","Průvodce nekonečnem"));
    collectionLabel->setText(text("01  /  COLLECTION","01  /  KOLEKCE"));
    controlsLabel->setText(text("02  /  EXPLORE","02  /  PROZKOUMAT"));
    lessonLabel->setText(text("03  /  FIELD NOTES","03  /  POZNÁMKY"));
    for (int i=0;i<4;++i) fractalList->item(i)->setText(fractalName(Fractal(i),czech));
    fractalList->setAccessibleName(text("Choose a fractal","Vyberte fraktál"));
    language->setAccessibleName(text("Language","Jazyk"));
    language->setToolTip(text("Interface and lesson language","Jazyk rozhraní a výuky"));
    title->setText(fractalName(selected,czech)); equation->setText(formula(selected));
    iterationsLabel->setText(text("Iteration limit","Limit iterací"));
    iterations->setToolTip(text("Higher limits reveal more boundary detail, but take longer to render.","Vyšší limit odhalí více detailů hranice, ale výpočet trvá déle."));
    iterations->setAccessibleName(iterationsLabel->text());
    reset->setText(text("Reset view","Výchozí pohled")); learnToggle->setText(text("Learning panel","Výukový panel"));
    zoomIn->setAccessibleName(text("Zoom in","Přiblížit")); zoomOut->setAccessibleName(text("Zoom out","Oddálit"));
    zoomIn->setToolTip(zoomIn->accessibleName()); zoomOut->setToolTip(zoomOut->accessibleName());
    hint->setText(text("DRAG to move<br><br>SCROLL to zoom at the pointer<br><br>ARROWS to move · P / O to zoom<br><br>Click the image to use keyboard controls.",
                       "TAŽENÍ posouvá obraz<br><br>KOLEČKO přibližuje u kurzoru<br><br>ŠIPKY posouvají · P / O přibližují a oddalují<br><br>Pro ovládání klávesnicí klikněte do obrazu."));
    juliaLabel->setText(text("Julia parameter · c","Juliův parametr · c"));
    realLabel->setText("Re(c)"); imagLabel->setText("Im(c)");
    realPart->setAccessibleName(text("Real part of c","Reálná část c"));
    imagPart->setAccessibleName(text("Imaginary part of c","Imaginární část c"));
    realPart->setToolTip(realPart->accessibleName()); imagPart->setToolTip(imagPart->accessibleName());
    realLabel->setToolTip(realPart->accessibleName()); imagLabel->setToolTip(imagPart->accessibleName());
    presets->setAccessibleName(text("Julia presets","Předvolby Juliovy množiny"));
    const QSignalBlocker block(presets); const int old=presets->currentIndex(); presets->clear();
    presets->addItems({text("Spirals","Spirály"),text("Basilica","Bazilika"),text("Dendrite","Dendrit"),text("Custom","Vlastní")}); presets->setCurrentIndex(old);
    tabs->setTabText(0,text("Beginner","Základy")); tabs->setTabText(1,text("Mathematics","Matematika")); tabs->setTabText(2,text("History","Historie"));
    tryButton->setText(text("Try this view  →","Vyzkoušet tento pohled  →"));
    tryDescription->setText(explorationText(selected,czech));
    refreshLessons();
}
void Window::refreshControls() {
    const auto &v=states[int(selected)];
    const QSignalBlocker a(iterations),b(realPart),c(imagPart),d(presets);
    iterations->setValue(v.iterations); realPart->setValue(v.julia.x()); imagPart->setValue(v.julia.y());
    juliaControls->setVisible(selected==Fractal::Julia);
    const QPointF values[]={{-.8,.156},{-1,0},{0,1}}; int index=3;
    for (int i=0;i<3;++i) if (v.julia==values[i]) index=i;
    presets->setCurrentIndex(index);
    const QLocale locale(czech?QLocale::Czech:QLocale::English);
    realPart->setLocale(locale); imagPart->setLocale(locale); iterations->setLocale(locale);
}
void Window::refreshLessons() {
    for (int i=0;i<3;++i) { pages[i]->setHtml(lessonHtml(selected,i,czech)); pages[i]->setAccessibleName(tabs->tabText(i)); }
}
void Window::parametersChanged() {
    states[1].julia={realPart->value(),imagPart->value()};
    viewer->setScene(selected,states[1]); refreshControls();
}
