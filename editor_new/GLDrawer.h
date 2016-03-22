/*
* GLDrawer.h
*
*  Created on: 27.11.2010
*      Author: fabian
*/

#ifndef GLDRAWER_H_
#define GLDRAWER_H_

#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>
#include <GL/GLU.h>
/*
Copyright 2010 by Fabian Schwartau
E-Mail: fabian@opencode.eu

This file is part of RepRapGCodeSimulator.

RepRapGCodeSimulator is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RepRapGCodeSimulator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BoostComPort.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui/QWheelEvent>
#include <iostream>

#include "GCodeInterpreter.h"

using namespace std;



class GLDrawer : public QGLWidget
{
	Q_OBJECT
public:
	GLDrawer(QWidget* parent = NULL);
	~GLDrawer();

	void loadGCode(string fileName);
	void changeLayer(int l);
	int MaxLayer;
	void setHeight(float h);
protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent* event);


private:
	void drawBox();
	void drawSphere(GLfloat xx, GLfloat yy, GLfloat zz, GLfloat radius, GLfloat M, GLfloat N);
	void RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, double radius);
	void mySolidCylinder(GLUquadric*   quad,
		GLdouble base,
		GLdouble top,
		GLdouble height,
		GLint slices,
		GLint stacks);
	void DrawCircleArea(float cx, float cy, float cz, float r, int num_segments);
	void DrawCircle(float x, float y, float z, double r, int slices, float norm_x, float norm_y, float norm_z);
	QPoint lastMousePosition;
	bool midMousePressed;
	bool leftMousePressed;
	bool rightMousePressed;
	int transX, transY, transZ;
	int rotX, rotY, rotZ;
	int selectLayer;
	float layerheight;
	GCodeInterpreter interpreter;
};




#endif /* GLDRAWER_H_ */
