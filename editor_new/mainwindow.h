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
#include "gcodeviewer.h"
#include "profile.h"
#include <stdlib.h>
#include<stdio.h>
#include   "windows.h "                      
#include   "shellapi.h " 
#include<cstring>
#include <QTextCodec>
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
	QPushButton *printSetting, *printPrint,*printPreview;
	Gcodeviewer *w;
	void slide();
	public slots:
	void open_setting();
	void begin_print();
	void print_preview();
};

#endif // MAINWINDOW_H
