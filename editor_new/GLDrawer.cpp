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

#include "GLDrawer.h"
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>
#include <QCoreApplication>
const float posi[41][3] = {
	{ -3, -3.2, -10 }, { -1.5, -3.2, -10 }, { 0, -3.2, -10 }, { 1.5, -3.2, -10 }, { 3, -3.2, -10 },
	{ -3, -1.6, -11 }, { -1.5, -1.6, -11 }, { 0, -1.6, -11 }, { 1.5, -1.6, -11 }, { 3, -1.6, -11 },
	{ -3.75, 0, -13 }, { -2.5, 0, -13 }, { -1.25, 0, -13 }, { 0, 0, -13 }, { 1.25, 0, -13 }, { 2.5, 0, -13 }, { 3.75, 0, -13 },
	{ -5.25, 1.9, -16 }, { -3.75, 1.9, -16 }, { -2.25, 1.9, -16 }, { -0.75, 1.9, -16 }, { 0.75, 1.9, -16 }, { 2.25, 1.9, -16 }, { 3.75, 1.9, -16 }, { 5.25, 1.9, -16 },
	{ -5.25, 3.7, -16 }, { -3.75, 3.7, -16 }, { -2.25, 3.7, -16 }, { -0.75, 3.7, -16 }, { 0.75, 3.7, -16 }, { 2.25, 3.7, -16 }, { 3.75, 3.7, -16 }, { 5.25, 3.7, -16 },
	{ -5.25, 5.6, -16 }, { -3.75, 5.6, -16 }, { -2.25, 5.6, -16 }, { -0.75, 5.6, -16 }, { 0.75, 5.6, -16 }, { 2.25, 5.6, -16 }, { 3.75, 5.6, -16 }, { 5.25, 5.6, -16 }
};

GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
GLfloat mat_diffuse[] = { 0.1, 0.5, 0.8, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat no_shininess[] = { 0.0 };
GLfloat low_shininess[] = { 5.0 };
GLfloat high_shininess[] = { 100.0 };
GLfloat mat_emission[] = { 0.3, 0.2, 0.2, 0.0 };
double PI = 3.1415926;

GLDrawer::GLDrawer(QWidget* parent) :
QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
lastMousePosition(0, 0),
midMousePressed(false),
leftMousePressed(false),
transX(0),
transY(0),
transZ(-160),
rotX(0),
rotY(0),
rotZ(0)
{
	setMouseTracking(true);
	resize(800, 600);
}

GLDrawer::~GLDrawer()
{

}

void GLDrawer::initializeGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);
	/*
	GLfloat light_ambient0[] = { 0, 0.5, 1, 1.0 };
	GLfloat light_diffuse0[] = { 0.0, 0.5, 1.0, 1.0 };
	GLfloat light_position0[] = { -1, -1, -1, 0.0 };
	GLfloat light_ambient1[] = { 0, 0.5, 1, 1.0 };
	GLfloat light_diffuse1[] = { 0.0, 0.5, 1.0, 1.0 };
	GLfloat light_position1[] = { 0, 0, 1.0, 0.0 };
	*/
	GLfloat light_ambient0[] = { 0, 0, 0, 1.0 };
	GLfloat light_diffuse0[] = { 1, 1, 1.0, 1.0 };
	GLfloat light_position0[] = { -1, -1, -1, 0.0 };
	GLfloat light_ambient1[] = { 0, 0, 0, 1.0 };
	GLfloat light_diffuse1[] = { 1, 1, 1.0, 1.0 };
	GLfloat light_position1[] = { 0, 1, 0.0, 0.0 };
	GLfloat light_ambient2[] = { 0, 0, 0, 1.0 };
	GLfloat light_diffuse2[] = { 1, 1, 1.0, 1.0 };
	GLfloat light_position2[] = { 0, -1, 0, 0.0 };

	/*
	GLfloat mat_shininess[] = { 50.0 };
	float mat_specular[] = { 0.0, 0.0, 1.0, 1.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);


	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	*/
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);
	glEnable(GL_DEPTH_TEST);

	//glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_NORMALIZE);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//cout<<"Init..."<<endl;
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);
	//glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	glEnable(GL_POLYGON_SMOOTH);
	glClearColor(0, 0, 0, 255);

}

void GLDrawer::resizeGL(int width, int height)
{
	//cout<<"resize..."<<endl;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, width, 0, height); // set origin to bottom left corner
	gluPerspective(90, (double)width / (double)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLDrawer::paintGL()
{
	//cout<<"Repaint..."<<endl;

	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(((float)transX) / 7.0, ((float)transY) / 7.0, (float)transZ);
	glRotatef(-rotX, 1, 0, 0);
	glRotatef(-rotY, 0, 1, 0);

	float dx = (interpreter.getMaxX() + interpreter.getMinX()) / 2.0;
	float dy = (interpreter.getMaxY() + interpreter.getMinY()) / 2.0;
	float dz = (interpreter.getMaxZ() + interpreter.getMinZ()) / 2.0;
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, -100);
	glVertex3f(0, 0, 100);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0, -100, 0);
	glVertex3f(0, 100, 0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-100, -0, 0);
	glVertex3f(100, 0, 0);
	glEnd();
	glTranslatef(-dx, -dy, -dz);
	glColor3f(0, 0, 1);

	if (!interpreter.isOpen())
	{
		glPopMatrix();
		return;
	}
	drawBox();
	/*
	glColor3f(0,1,0);

	glBegin(GL_LINES);
	glVertex3f(0,0,-20);
	glVertex3f(0,0,20);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0,-20,0);
	glVertex3f(0,20,0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-20,-0,0);
	glVertex3f(20,0,0);
	glEnd();

	glColor3f(1,1,0);
	*/
	vector<GCodeLine>& codeLines = interpreter.getCodeLines();
	float oldx = 0, oldy = 0, oldz = 0;


	for (unsigned int i = 0; i<codeLines.size(); i++)
	{
		if (codeLines[i].command == "G1" && codeLines[i].layer < selectLayer)  // draw a line
		{
			float x = oldx, y = oldy, z = oldz;
			bool hasE = false;
			for (unsigned int j = 0; j<codeLines[i].parameters.size(); j++)
			{
				if (codeLines[i].parameters[j].name == 'E')
					hasE = true;
			}
			for (unsigned int j = 0; j<codeLines[i].parameters.size(); j++)
			{
				if (codeLines[i].parameters[j].name == 'X')
					x = codeLines[i].parameters[j].value;
				if (codeLines[i].parameters[j].name == 'Y')
					y = codeLines[i].parameters[j].value;
				if (codeLines[i].parameters[j].name == 'Z')
					z = codeLines[i].parameters[j].value;


			}
			if (hasE)
			{
				glColor3f(1, 1, 1);
				RenderBone(oldx, oldy, oldz, x, y, z, 0.5 * layerheight);
				//drawSphere(oldx, oldy, oldz, 0.1, 3, 3);
				//drawSphere(x, y, z, 0.1, 3, 3);
				/*glBegin(GL_LINES);

				glVertex3f(oldx, oldy, oldz);
				glVertex3f(x, y, z);

				glEnd();
				*/
			}

			oldx = x;
			oldy = y;
			oldz = z;
		}
	}

	glPopMatrix();
}

void GLDrawer::drawBox()
{
	//RenderBone(interpreter.getMinX(), interpreter.getMinY(), interpreter.getMinZ(), interpreter.getMinX(), interpreter.getMinY(), interpreter.getMaxZ(), 0.1);
	glBegin(GL_LINES);
	// vorne

	glVertex3f(interpreter.getMinX(), interpreter.getMinY(), interpreter.getMinZ());
	glVertex3f(interpreter.getMinX(), interpreter.getMinY(), interpreter.getMaxZ());

	glVertex3f(interpreter.getMinX(), interpreter.getMinY(), interpreter.getMaxZ());
	glVertex3f(interpreter.getMaxX(), interpreter.getMinY(), interpreter.getMaxZ());

	glVertex3f(interpreter.getMaxX(), interpreter.getMinY(), interpreter.getMaxZ());
	glVertex3f(interpreter.getMaxX(), interpreter.getMinY(), interpreter.getMinZ());

	glVertex3f(interpreter.getMaxX(), interpreter.getMinY(), interpreter.getMinZ());
	glVertex3f(interpreter.getMinX(), interpreter.getMinY(), interpreter.getMinZ());

	// hinten
	glVertex3f(interpreter.getMinX(), interpreter.getMaxY(), interpreter.getMinZ());
	glVertex3f(interpreter.getMinX(), interpreter.getMaxY(), interpreter.getMaxZ());

	glVertex3f(interpreter.getMinX(), interpreter.getMaxY(), interpreter.getMaxZ());
	glVertex3f(interpreter.getMaxX(), interpreter.getMaxY(), interpreter.getMaxZ());

	glVertex3f(interpreter.getMaxX(), interpreter.getMaxY(), interpreter.getMaxZ());
	glVertex3f(interpreter.getMaxX(), interpreter.getMaxY(), interpreter.getMinZ());

	glVertex3f(interpreter.getMaxX(), interpreter.getMaxY(), interpreter.getMinZ());
	glVertex3f(interpreter.getMinX(), interpreter.getMaxY(), interpreter.getMinZ());

	// seiten
	glVertex3f(interpreter.getMinX(), interpreter.getMinY(), interpreter.getMinZ());
	glVertex3f(interpreter.getMinX(), interpreter.getMaxY(), interpreter.getMinZ());

	glVertex3f(interpreter.getMaxX(), interpreter.getMinY(), interpreter.getMinZ());
	glVertex3f(interpreter.getMaxX(), interpreter.getMaxY(), interpreter.getMinZ());

	glVertex3f(interpreter.getMinX(), interpreter.getMinY(), interpreter.getMaxZ());
	glVertex3f(interpreter.getMinX(), interpreter.getMaxY(), interpreter.getMaxZ());

	glVertex3f(interpreter.getMaxX(), interpreter.getMinY(), interpreter.getMaxZ());
	glVertex3f(interpreter.getMaxX(), interpreter.getMaxY(), interpreter.getMaxZ());

	glEnd();
}

void GLDrawer::mouseReleaseEvent(QMouseEvent* event)
{
	//cout<<"Mouse released"<<endl;
	if (event->button() == Qt::LeftButton)
		leftMousePressed = false;
	else if (event->button() == Qt::MidButton)
		midMousePressed = false;
	else if (event->button() == Qt::RightButton)
		rightMousePressed = false;
}

void GLDrawer::mousePressEvent(QMouseEvent* event)
{
	//cout<<"Mouse pressed"<<endl;
	lastMousePosition = event->pos();
	if (event->button() == Qt::LeftButton)
		leftMousePressed = true;
	else if (event->button() == Qt::MidButton)
		midMousePressed = true;
	else if (event->button() == Qt::RightButton)
		rightMousePressed = true;
}
void GLDrawer::mouseMoveEvent(QMouseEvent* event)
{
	//cout<<transX<<"/"<<transY<<endl;
	if (midMousePressed)
	{
		transX += -(lastMousePosition.x() - event->x());
		transY += lastMousePosition.y() - event->y();
		updateGL();
	}
	if (leftMousePressed)
	{
		rotX += (lastMousePosition.y()) - event->y();
		rotY += (lastMousePosition.x()) - event->x();
		//cout<<"rotY: "<<rotY<<endl;
		updateGL();
	}
	lastMousePosition = event->pos();
}

void GLDrawer::wheelEvent(QWheelEvent* event)
{
	transZ += event->delta() / 30.0;
	//cout<<"transZ: "<<transZ<<endl;
	updateGL();
}

void GLDrawer::loadGCode(string fileName)
{
	interpreter.clear();
	interpreter.open(fileName);
	MaxLayer = interpreter.MaxLayer;
	changeLayer(MaxLayer);
}

void GLDrawer::changeLayer(int l){
	selectLayer = l;
	updateGL();
}




void GLDrawer::RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, double radius)
{
	GLdouble  dir_x = x1 - x0;
	GLdouble  dir_y = y1 - y0;
	GLdouble  dir_z = z1 - z0;
	GLdouble  bone_length = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
	static GLUquadricObj *  quad_obj = NULL;
	if (quad_obj == NULL)
		quad_obj = gluNewQuadric();
	gluQuadricDrawStyle(quad_obj, GLU_FILL);
	gluQuadricNormals(quad_obj, GLU_SMOOTH);
	glPushMatrix();
	// 平移到起始点  
	glTranslated(x0, y0, z0);
	// 计算长度  
	double  length;
	length = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
	if (length < 0.0001) {
		dir_x = 0.0; dir_y = 0.0; dir_z = 1.0;  length = 1.0;
	}
	dir_x /= length;  dir_y /= length;  dir_z /= length;
	GLdouble  up_x, up_y, up_z;
	up_x = 0.0;
	up_y = 1.0;
	up_z = 0.0;
	double  side_x, side_y, side_z;
	side_x = up_y * dir_z - up_z * dir_y;
	side_y = up_z * dir_x - up_x * dir_z;
	side_z = up_x * dir_y - up_y * dir_x;
	length = sqrt(side_x*side_x + side_y*side_y + side_z*side_z);
	if (length < 0.0001) {
		side_x = 1.0; side_y = 0.0; side_z = 0.0;  length = 1.0;
	}
	side_x /= length;  side_y /= length;  side_z /= length;
	up_x = dir_y * side_z - dir_z * side_y;
	up_y = dir_z * side_x - dir_x * side_z;
	up_z = dir_x * side_y - dir_y * side_x;
	// 计算变换矩阵  
	GLdouble  m[16] = { side_x, side_y, side_z, 0.0,
		up_x, up_y, up_z, 0.0,
		dir_x, dir_y, dir_z, 0.0,
		0.0, 0.0, 0.0, 1.0 };
	glMultMatrixd(m);
	// 圆柱体参数  
	//GLdouble radius = 20;        // 半径  
	GLdouble slices = 8.0;      //  段数  
	GLdouble stack = 3.0;       // 递归次数  
	//mySolidCylinder(quad_obj, radius, radius, bone_length, slices, stack);
	gluCylinder(quad_obj, radius, radius, bone_length, slices, stack);
	glPopMatrix();
}

void GLDrawer::mySolidCylinder(GLUquadric*   quad,
	GLdouble base,
	GLdouble top,
	GLdouble height,
	GLint slices,
	GLint stacks)
{
	//glColor3f(84.0 / 255, 0.0, 125.0 / 255.0);
	gluCylinder(quad, base, top, height, slices, stacks);
	//top   
	DrawCircleArea(0.0, 0.0, height, top, slices);
	//base   
	DrawCircleArea(0.0, 0.0, 0.0, base, slices);
}

void GLDrawer::DrawCircle(float x, float y, float z, double r, int slices, float norm_x, float norm_y, float norm_z){

}
void GLDrawer::DrawCircleArea(float cx, float cy, float cz, float r, int num_segments)
{
	GLfloat vertex[4];

	const GLfloat delta_angle = 2.0*PI / num_segments;
	glBegin(GL_TRIANGLE_FAN);

	vertex[0] = cx;
	vertex[1] = cy;
	vertex[2] = cz;
	vertex[3] = 1.0;
	glVertex4fv(vertex);

	//draw the vertex on the contour of the circle   
	for (int i = 0; i < num_segments; i++)
	{
		vertex[0] = std::cos(delta_angle*i) * r + cx;
		vertex[1] = std::sin(delta_angle*i) * r + cy;
		vertex[2] = cz;
		vertex[3] = 1.0;
		glVertex4fv(vertex);
	}

	vertex[0] = 1.0 * r + cx;
	vertex[1] = 0.0 * r + cy;
	vertex[2] = cz;
	vertex[3] = 1.0;
	glVertex4fv(vertex);
	glEnd();
}

void  GLDrawer::setHeight(float h){
	layerheight = h;
};