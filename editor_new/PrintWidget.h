#ifndef PRINTWIDGET_H
#define PRINTWIDGET_H
#pragma once
#include <QWidget>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QFileDialog>
#include "MyMesh.h"
#include "View2D.h"
#include "View3D_.h"
#include "VertexItem.h"
#include "qdebug.h"
#include <QWidget>
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

class PrintWidget :
	public QWidget
{
	Q_OBJECT
public:
	PrintWidget();
	~PrintWidget();

	View2D *viewX, *viewY, *viewZ;
	View3D_ *view3D;
	QGraphicsScene *sceneX, *sceneY, *sceneZ;
	MyMesh *mm;
	QProgressBar *progress;
	QPushButton *openButton, *saveAsButton, *saveButton, *deleteButton, *normButton, *sketchButton;
	QLabel *status;
	bool loadFromFile;
	QString saveName;

	void init();

	
private:
	QPushButton *printSetting, *printPrint, *printPreview, *rotate, *view, *save_button;
	Gcodeviewer *w;
	int display_mode;
	float slide();

	public slots:
	void open_setting();
	void begin_print();
	void print_preview();
	void rolate_mode(){
		display_mode = 2;
		view3D->selectMode = display_mode;
	};
	void view_mode(){
		display_mode = 1;
		view3D->selectMode = display_mode;
	};
	void save();
	//void change_pos();

	
};

#endif // PRINTWIDGET_H