#include "Sketch3D.h"

extern string inFileName;
extern string outFileName;
extern Image myImage;

Sketch3D::Sketch3D()
{
	
}


Sketch3D::~Sketch3D()
{
}

void Sketch3D::initializeGL()
{
	GLfloat light_ambient0[] = { 0, 0, 0, 1.0 };
	GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position0[] = { -1, -1, -1, 0.0 };
	GLfloat light_ambient1[] = { 0, 0, 0, 1.0 };
	GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position1[] = { 0, 0, 1.0, 0.0 };
	GLfloat light_ambient2[] = { 0, 0, 0, 1.0 };
	GLfloat light_diffuse2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position2[] = { 1.0, 0.0, -1.0, 0.0 };

	GLfloat mat_shininess[] = { 50.0 };
	//float no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	//float mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	//float mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
	//float mat_diffuse[] = { 0.0, 0.7, 0.0, 1.0 };
	float mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	//float no_shininess[] = { 0.0 };
	//float low_shininess[] = { 5.0 };
	//float high_shininess[] = { 50.0 };
	//float mat_emission[] = {0.0, 0.0, 0.0, 0.0};

	//glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
	//glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
	//glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);


	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);

	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

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
	glColor3f(0.0, 0.0, 0.0);
}

void Sketch3D::paintGL()
{
	glColor3f(0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glScalef(myImage.scale3D, myImage.scale3D, myImage.scale3D);
	glRotatef(myImage.anglex3D, 1.0, 0.0, 0.0);
	glRotatef(myImage.angley3D, 0.0, 1.0, 0.0);
	glRotatef(myImage.anglez3D, 0.0, 0.0, 1.0);

	myImage.drawShape();

	glFlush();

	SwapBuffers(wglGetCurrentDC());
}

void Sketch3D::resizeGL(int w, int h)
{
	myImage.width3D = w; myImage.height3D = h;
	cout << "width: " << myImage.width3D << ", height: " << myImage.height3D << endl;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-1, 1, -1 * (GLfloat)h / (GLfloat)w,
		1 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-1 * (GLfloat)w / (GLfloat)h,
		1 * (GLfloat)w / (GLfloat)h, -1, 1, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);

	glDrawBuffer(GL_BACK);
}

void Sketch3D::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_B:
		myImage.bigger();
		glutPostRedisplay();
		break;
	case Qt::Key_S:
		myImage.smaller();
		glutPostRedisplay();
		break;
	case Qt::Key_R:
		myImage.turnS();
		glutPostRedisplay();
		break;
	case Qt::Key_L:
		myImage.turnN();
		glutPostRedisplay();
		break;
	case Qt::Key_O:					//导出到文件
		outFileName = inFileName + ".off";
		myImage.myShape.outToFile(outFileName);
		break;
	case Qt::Key_Left:
		myImage.turnLeft();
		glutPostRedisplay();
		break;
	case Qt::Key_Right:
		myImage.turnRight();
		glutPostRedisplay();
		break;
	case Qt::Key_Up:
		myImage.turnUp();
		glutPostRedisplay();
		break;
	case Qt::Key_Down:
		myImage.turnDown();
		glutPostRedisplay();
		break;

	}
}