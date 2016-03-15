#ifndef GCODEVIEWER_H
#define GCODEVIEWER_H


#include <QMainWindow>
#include <QSlider> 
#include <QHBoxLayout> 
//#include "ui_MainWindow.h"
#include "GLDrawer.h"
#include "DockOpenGL.h"
#include "qpushbutton.h"
#include "qaction.h"
#include "qtoolbar.h"
#include "qmenu.h"

class Gcodeviewer : public QMainWindow
{
    Q_OBJECT

public:
    Gcodeviewer(QWidget *parent = 0);
    ~Gcodeviewer();
    void loadGCode(string fileName);
private:
   // Ui::QtOpenGLClass ui;
    DockOpenGL* GLDock;
    QPushButton *loadgcode;
    QAction *openAct;
    QMenu *fileMenu;
    QToolBar *fileToolBar;
	QSlider *layerselect;

private slots:
    void onBtLoadGCode();
	void changeLayer();

};

#endif // GCODEVIEWER_H
