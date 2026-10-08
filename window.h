#pragma once
#include "viewer.h"
#include <QMainWindow>
#include <array>
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QListWidget;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QTextBrowser;
class QSplitter;

class Window : public QMainWindow {
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
    Viewer *canvas() const { return viewer; }
    void setCzech(bool enabled);
    bool isCzech() const { return czech; }
private:
    QString text(const char *en, const char *cs) const;
    void selectFractal(int index);
    void refreshTexts();
    void refreshControls();
    void refreshLessons();
    void parametersChanged();
    bool czech = false;
    Fractal selected = Fractal::Mandelbrot;
    std::array<View,4> states;
    Viewer *viewer;
    QWidget *learning, *juliaControls;
    QSplitter *splitter;
    QListWidget *fractalList;
    QComboBox *language, *presets;
    QSpinBox *iterations;
    QDoubleSpinBox *realPart, *imagPart;
    QPushButton *reset, *learnToggle, *tryButton, *zoomIn, *zoomOut;
    QLabel *collectionLabel, *controlsLabel, *iterationsLabel, *hint, *title, *equation,
           *lessonLabel, *tryDescription, *juliaLabel, *realLabel, *imagLabel, *subtitle;
    QTabWidget *tabs;
    std::array<QTextBrowser*,3> pages;
};
