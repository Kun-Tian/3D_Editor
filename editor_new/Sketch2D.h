#pragma once
#include <QGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include "drawing.h"

class Sketch2D :
	public QGLWidget
{
public:
	Sketch2D();
	~Sketch2D();

	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};

