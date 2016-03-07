#include <QtGui>
#include <QtOpenGL>
#include <QDebug>
#include <cmath>
#include <QString>
#include "View3D.h"

//#include "GL/GLU.H"
//#include "GL/glut.h"

#ifndef PI
#define PI 3.1415926536
#endif

GLint View3D::_colorList[12][3] = {
		{ 196, 196, 196 }, //无分割信息情况下默认显示颜色
		{ 220, 20, 60 }, //浅粉红
		{ 255, 215, 0 }, //金色
		{ 128, 0, 128 }, //紫色
		{ 165, 42, 42 }, //棕色
		{ 0, 0, 205 }, //中蓝色
		{ 160, 82, 45 }, //黄土赭色
		{ 0, 100, 0 }, //暗绿色
		{ 0, 191, 255 }, //深天蓝
		{ 0, 255, 127 }, //春绿色
		{ 255, 165, 0 }, //橙色
		{ 128, 128, 128 }
};

View3D::View3D(QWidget *parent) :
QGLWidget(parent)
{
	m = 3;

	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

	rotationX = 0.0;
	rotationY = 0.0;
	rotationZ = 0.0;

	setFocusPolicy(Qt::StrongFocus);

	mm = NULL;
	drawOriginal = true;
	drawBoundingBox = false;
	drawPartAxis = false;
	drawAxis = false;
	drawSelection = false;

	selectMode = false;
	part1 = "";
	part2 = "";
	
}

void View3D::setModel(MyMesh *m)
{
	mm = m;
	/*
	m = 3;
	rotationX = 0.0;
	rotationY = 0.0;
	rotationZ = 0.0;
	*/
	updateGL();
}

void View3D::captureDepth()
{
	//int NbBytes = size().width() * size().height();
	//float *pPixelData = new float[NbBytes];
	//glReadPixels(0, 0, size().width(), size().height(), GL_DEPTH_COMPONENT, GL_FLOAT, pPixelData);
	//QImage Image(pPixelData, size().width(), size().height(), NbBytes / size().height(), QImage::Format_RGB888);
	//Image.save("2.jpg");

}

void View3D::initializeGL()
{
	//glClearColor(80.0 / 255.0, 120.0 / 255.0, 220.0 / 255.0, 0.0f);
	glClearColor(240.0 / 255.0, 240.0 / 255.0, 240.0 / 255.0, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	init_light();
}	

void View3D::init_light()
{
	GLfloat white_light[] = { 0.23, 0.23, 0.23, 1.0 };

	//µÆ0ÓëµÆ1¾ùÔÚÎÞÇîÔ¶´¦
	GLfloat light_position0[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_position1[] = { 1.0, 1.0, -1.0, 0.0 };
	GLfloat light_position2[] = { 1.0, -1.0, 1.0, 0.0 };
	GLfloat light_position3[] = { 1.0, -1.0, -1.0, 0.0 };
	GLfloat light_position4[] = { -1.0, 1.0, 1.0, 0.0 };
	GLfloat light_position5[] = { -1.0, -1.0, 1.0, 0.0 };
	GLfloat light_position6[] = { -1.0, 1.0, -1.0, 0.0 };
	GLfloat light_position7[] = { -1.0, -1.0, -1.0, 0.0 };

	//ÉèÖÃÏàÓ¦µÄ¹âÔ´
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0); glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light); glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1); glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light); glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position2); glLightfv(GL_LIGHT2, GL_DIFFUSE, white_light); glLightfv(GL_LIGHT2, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT3, GL_POSITION, light_position3); glLightfv(GL_LIGHT3, GL_DIFFUSE, white_light); glLightfv(GL_LIGHT3, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT4, GL_POSITION, light_position4); glLightfv(GL_LIGHT4, GL_DIFFUSE, white_light); glLightfv(GL_LIGHT4, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT5, GL_POSITION, light_position5); glLightfv(GL_LIGHT5, GL_DIFFUSE, white_light); glLightfv(GL_LIGHT5, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT6, GL_POSITION, light_position6); glLightfv(GL_LIGHT6, GL_DIFFUSE, white_light); glLightfv(GL_LIGHT6, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT7, GL_POSITION, light_position7); glLightfv(GL_LIGHT7, GL_DIFFUSE, white_light); glLightfv(GL_LIGHT7, GL_SPECULAR, white_light);

	//ÆôÓÃ¹âÕÕÓëµÆ0µÆ1
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);
	glEnable(GL_LIGHT6);
	glEnable(GL_LIGHT7);

	//Éî¶È»º´æÉèÖÃ
	//glEnable(GL_DEPTH_TEST);
}

void View3D::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(80.0 / 255.0, 120.0 / 255.0, 220.0 / 255.0, 0.0f);
	glClearColor(240.0 / 255.0, 240.0 / 255.0, 240.0 / 255.0, 0.0f);
	drawTriangle();
	glFlush();
	glFinish();
}

void View3D::drawTriangle()
{
	GLfloat no_mat[4] = { 0.0, 0.0, 0, 1 };
	GLfloat mat_diffuse[4] = { 0, 0, 0, 1 };		//r±íÊ¾´óÖµ£¬b±íÊ¾Ð¡Öµ
	GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
	float no_shininess[4] = { 1, 1, 1, 0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear The Screen And The Depth Buffer
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -0.0);
	glRotatef(rotationX, 1.0, 0.0, 0.0);
	glRotatef(rotationY, 0.0, 1.0, 0.0);
	glRotatef(rotationZ, 0.0, 0.0, 1.0);

	double pi = acos(-1.0);

	mat_diffuse[0] = 1;
	mat_diffuse[1] = 0;
	mat_diffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (double i = 0; i <= 2 * pi; i += pi / 180)
	{
		double vy = m * cos(i);
		double vz = m * sin(i);
		glVertex3d(0, vy, vz);
	}
	glEnd();

	mat_diffuse[0] = 0;
	mat_diffuse[1] = 1;
	mat_diffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (double i = 0; i <= 2 * pi; i += pi / 180)
	{
		double vx = m * cos(i);
		double vz = m * sin(i);
		glVertex3d(vx, 0, vz);
	}
	glEnd();

	mat_diffuse[0] = 0;
	mat_diffuse[1] = 0;
	mat_diffuse[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 5);
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (double i = 0; i <= 2 * pi; i += pi / 180)
	{
		double vx = m * cos(i);
		double vy = m * sin(i);
		glVertex3d(vx, vy, 0);
	}
	glEnd();

	if (!mm)
		return;

	int count = 0;

	glBegin(GL_TRIANGLES);
	count = mm->faces.size();
	for (int i = 0; i < count; ++i)
	{
		if (mm->deletedF[i])
			continue;
		mat_diffuse[0] = _colorList[0][0] / 250.0;
		mat_diffuse[1] = _colorList[0][1] / 250.0;
		mat_diffuse[2] = _colorList[0][2] / 250.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		point a, b, n;
		a = mm->vertices[mm->faces[i][0]] - mm->vertices[mm->faces[i][1]];
		b = mm->vertices[mm->faces[i][2]] - mm->vertices[mm->faces[i][1]];
		n = a CROSS b;
		float temp1 = len(n);
		glNormal3f(n[0] / temp1, n[1] / temp1, n[2] / temp1);
		for (int k = 0; k < 3; k++) {
			glVertex3f(mm->vertices[mm->faces[i][k]][0] * m,
				mm->vertices[mm->faces[i][k]][1] * m,
				mm->vertices[mm->faces[i][k]][2] * m);
		}
	}

	mat_diffuse[0] = _colorList[1][0] / 250.0;
	mat_diffuse[1] = _colorList[1][1] / 250.0;
	mat_diffuse[2] = _colorList[1][2] / 250.0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	for (int i = 0; i < mm->vertices.size(); i++)
	{
		if (mm->selected[i])
		{
			drawSphere(mm->vertices[i][0] * m, mm->vertices[i][1] * m, mm->vertices[i][2] * m, 0.005*m, 3, 3);
		}
	}
	glEnd();

}

void View3D::drawSphere(GLfloat xx, GLfloat yy, GLfloat zz, GLfloat radius, GLfloat M, GLfloat N)
{
	float step_z = PI / M;
	float step_xy = 2 * PI / N;
	float x[4], y[4], z[4];

	float angle_z = 0.0;
	float angle_xy = 0.0;
	int i = 0, j = 0;
	glBegin(GL_QUADS);

	for (i = 0; i<M; i++)
	{
		angle_z = i * step_z;

		for (j = 0; j<N; j++)
		{
			angle_xy = j * step_xy;

			x[0] = radius * sin(angle_z) * cos(angle_xy);
			y[0] = radius * sin(angle_z) * sin(angle_xy);
			z[0] = radius * cos(angle_z);

			x[1] = radius * sin(angle_z + step_z) * cos(angle_xy);
			y[1] = radius * sin(angle_z + step_z) * sin(angle_xy);
			z[1] = radius * cos(angle_z + step_z);

			x[2] = radius*sin(angle_z + step_z)*cos(angle_xy + step_xy);
			y[2] = radius*sin(angle_z + step_z)*sin(angle_xy + step_xy);
			z[2] = radius*cos(angle_z + step_z);

			x[3] = radius * sin(angle_z) * cos(angle_xy + step_xy);
			y[3] = radius * sin(angle_z) * sin(angle_xy + step_xy);
			z[3] = radius * cos(angle_z);

			for (int k = 0; k<4; k++)
			{
				glVertex3f(xx + x[k], yy + y[k], zz + z[k]);
			}
		}
	}
	glEnd();
}

void View3D::resizeGL(int width, int height)
{
	GLfloat nRange = 3.0f;
	if (height == 0) {    // Prevent A Divide By Zero By  
		height = 1;    // Making Height Equal One  
	}
	glViewport(0, 0, width, height);    // Reset The Current Viewport  
	glMatrixMode(GL_PROJECTION);       // Select The Projection Matrix  
	glLoadIdentity();                  // Reset The Projection Matrix  

	if (width <= height)
		glOrtho(-nRange, nRange, -nRange*height / width, nRange*height / width, -nRange, nRange);
	else
		glOrtho(-nRange*width / height, nRange*width / height, -nRange, nRange, -nRange, nRange);
	glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix  
	glLoadIdentity();
}

void View3D::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
	clickEvent = true;
}

void View3D::mouseMoveEvent(QMouseEvent *event)
{
	clickEvent = false;
	GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
	GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();

	if (event->buttons() & Qt::LeftButton) {
		rotationX += 180 * dy;
		rotationY += 180 * dx;
		updateGL();
	}
	else if (event->buttons() & Qt::RightButton) {
		rotationX += 180 * dy;
		rotationZ += 180 * dx;
		updateGL();
	}
	lastPos = event->pos();
}

void View3D::mouseReleaseEvent(QMouseEvent *event)
{
	qDebug() << "X: " << rotationX;
	qDebug() << "Y: " << rotationY;
	qDebug() << "Z: " << rotationZ << endl;

	return;
	if (!clickEvent)
		return;
	if (event->button() == Qt::LeftButton)
		qDebug() << "left clicked";
	else if (event->button() == Qt::RightButton)
		qDebug() << "right clicked";

	/*
	GLuint pickBuffer[512];
	GLint nPicks, vp[4];
	glSelectBuffer(512, pickBuffer); //ÉèÖÃÑ¡Ôñ»º³åÇø
	glRenderMode(GL_SELECT); //¼¤»îÑ¡ÔñÄ£Ê½
	glInitNames(); //³õÊ¼»¯Ãû×Ö¶ÑÕ»
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glGetIntegerv(GL_VIEWPORT, vp);
	//¶¨ÒåÒ»¸ö10¡Á10µÄÑ¡ÔñÇøÓò
	gluPickMatrix(GLdouble(event->x()), GLdouble(vp[3] - event->y()), 10.0, 10.0, vp);
	gluOrtho2D(0.0, width(), 0.0, height());
	drawTriangle();
	//»Ö¸´Í¶Ó°±ä»»
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glFlush();
	//»ñµÃÑ¡Ôñ¼¯²¢Êä³ö
	nPicks = glRenderMode(GL_RENDER);
	ProcessPicks(nPicks, pickBuffer);
	//glutPostRedisplay();
	updateGL();*/
}

void View3D::ProcessPicks(GLint nPicks, GLuint pickBuffer[])
{
	GLint i;
	GLuint name, *ptr;
	qDebug() << "Selected " << nPicks << " Objects";
	ptr = pickBuffer;
	for (i = 0; i < nPicks; i++)
	{
		name = *ptr; //Ñ¡ÖÐÍ¼ÔªÔÚ¶ÑÕ»ÖÐµÄÎ»ÖÃ
		ptr += 3; //Ìø¹ýÃû×ÖºÍÉî¶ÈÐÅÏ¢
		ptr += name - 1; //¸ù¾ÝÎ»ÖÃÐÅÏ¢»ñµÃÑ¡ÖÐµÄÍ¼ÔªÃû×Ö
		qDebug() << *ptr;
		ptr++;
	}
}

void View3D::wheelEvent(QWheelEvent *e)
{
	if (e->delta() > 0 && m > 0.3)
		m *= 0.9f;
	if (e->delta() < 0 && m < 30)
		m *= 1.1f;
	updateGL();
}

void View3D::normalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

void View3D::setXRotation(int angle)
{
	normalizeAngle(angle);
	if (angle != rotationX) {
		rotationX = angle;
		emit xRotationChanged(angle);
		updateGL();
	}
}

void View3D::setYRotation(int angle)
{
	normalizeAngle(angle);
	if (angle != rotationY) {
		rotationY = angle;
		emit yRotationChanged(angle);
		updateGL();
	}
}

void View3D::setZRotation(int angle)
{
	normalizeAngle(angle);
	if (angle != rotationY) {
		rotationY = angle;
		emit zRotationChanged(angle);
		updateGL();
	}
}

void View3D::resetView()
{
	m = 3;
	rotationX = 0.0;
	rotationY = 0.0;
	rotationZ = 0.0;
	updateGL();
}