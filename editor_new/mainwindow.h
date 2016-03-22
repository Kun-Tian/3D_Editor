#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "MyMesh.h"
#include "EditWidget.h"
#include "PrintWidget.h"
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
	/*
	QPushButton *printSetting, *printPrint, *printPreview, *rolate, *view;
	Gcodeviewer *w;
	int display_mode;
	float slide();
	public slots:
	void open_setting();
	void begin_print();
	void print_preview();
	void rolate_mode(){ 
		display_mode = 1; 
	};
	void view_mode(){
		display_mode = 2;
	};*/
};

#endif // MAINWINDOW_H
