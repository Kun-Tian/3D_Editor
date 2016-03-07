#pragma once
#include <QGLWidget>
#include "MyMesh.h"

using namespace trimesh;

class View3D :
	public QGLWidget
{
	Q_OBJECT

public:
	explicit View3D(QWidget *parent = 0);

	int xRotation() const { return rotationX; }
	int yRotation() const { return rotationY; }
	int zRotation() const { return rotationZ; }
	void setModel(MyMesh *m);
	void resetView();
	void captureDepth();

	bool drawOriginal, drawBoundingBox, drawPartAxis, drawAxis, drawSelection;
	QStringList partNames;
	QString part1, part2;

signals:
	void xRotationChanged(int angle);
	void yRotationChanged(int angle);
	void zRotationChanged(int angle);

	public slots:
	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);

protected:
	void initializeGL();
	void paintGL();
	void init_light();
	void resizeGL(int w, int h);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *e);

	private slots:
	void drawTriangle();

private:
	void normalizeAngle(int &angle);
	void drawSphere(GLfloat xx, GLfloat yy, GLfloat zz, GLfloat radius, GLfloat M, GLfloat N);
	void ProcessPicks(GLint nPicks, GLuint pickBuffer[]);

	static GLint _colorList[12][3];

	GLfloat rotationX;
	GLfloat rotationY;
	GLfloat rotationZ;
	float m;
	MyMesh *mm;

	QPoint lastPos;
	bool clickEvent;
	bool selectMode;
};

