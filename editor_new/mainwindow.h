#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "MyMesh.h"
#include "EditWidget.h"
#include <QTabWidget>
#include <QPushButton>
#include"setting.h"
#include "ClientNet.h"  
#include "off2stl.h"
#include "qmessagebox.h"
#include "SketchWidget.h"

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QSplitter;
QT_END_NAMESPACE

using namespace trimesh;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);

private:
	QTabWidget *tab;
	QPushButton *printSetting, *printPrint;

	public slots:
	void open_setting();
	void begin_print();
};

#endif // MAINWINDOW_H
