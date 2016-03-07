#pragma once
#include <QWidget>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include "MyMesh.h"
#include "View2D.h"
#include "View3D.h"
#include "VertexItem.h"
#include "qdebug.h"

class EditWidget :
	public QWidget
{
	Q_OBJECT
public:
	EditWidget();
	~EditWidget();

	View2D *viewX, *viewY, *viewZ;
	View3D *view3D;
	QGraphicsScene *sceneX, *sceneY, *sceneZ;
	MyMesh *mm;
	QProgressBar *progress;
	QPushButton *openButton, *saveButton,*deleteButton,*normButton;
	QLabel *status;

public slots:
	void init();
	void save();
	void deleteVertex();
	void norm();
};
