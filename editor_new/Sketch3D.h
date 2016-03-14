#pragma once
#include <QGLWidget>
#include <QKeyEvent>
#include "drawing.h"

class Sketch3D :
	public QGLWidget
{
public:
	Sketch3D();
	~Sketch3D();

	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void keyPressEvent(QKeyEvent *e);
};

