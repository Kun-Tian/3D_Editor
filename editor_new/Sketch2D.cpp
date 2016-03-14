#include "Sketch2D.h"

extern double fov;
extern bool draw_mesh;
extern bool draw_faded;
extern bool draw_sc;									//
extern bool draw_sh;									//
extern bool draw_DwKr;
extern bool draw_apparent;							//
extern bool draw_extsil;								//
extern bool draw_hidden;
extern bool draw_colors;
extern bool draw_K;
extern bool draw_H;
extern bool draw_edges;
extern bool draw_ridges;								//
extern bool draw_valleys;							//
extern bool draw_phridges, draw_phvalleys;		//
extern bool draw_bdy;
extern bool draw_c;									//
extern bool draw_isoph;
extern bool draw_topo;
extern bool test_ar, test_c, test_sc, test_rv, test_ph, test_sh;
extern bool use_hermite;
extern float rv_thresh, ar_thresh;
extern float sug_thresh, sh_thresh, ph_thresh;
extern int mx, my;
extern vector3df viewVec;
extern Image myImage;

Sketch2D::Sketch2D()
{
}


Sketch2D::~Sketch2D()
{
}

void Sketch2D::initializeGL()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);

	glShadeModel(GL_FLAT);

	glColor3f(0.0, 0.0, 0.0);
}

void Sketch2D::paintGL()
{
	glColor3f(0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glLineWidth(1.0);
	myImage.drawSil();

	glColor3f(0.0, 0.0, 0.0);

	glLineWidth(2.0);
	myImage.drawStroke(myImage.defStroke3D);
	myImage.drawStroke(myImage.reDefStroke3D);

	//glColor3f(0.0, 1.0, 0.0);
	glLineWidth(3.0);
	myImage.drawStroke(myImage.matchSil3D);


	glFlush();


	SwapBuffers(wglGetCurrentDC());
}

void Sketch2D::resizeGL(int w, int h)
{
	myImage.width = w; myImage.height = h;
	cout << "width: " << myImage.width << ", height: " << myImage.height << endl;
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


	glGetIntegerv(GL_VIEWPORT, myImage.viewport);

	glDrawBuffer(GL_BACK);
}

void Sketch2D::keyPressEvent(QKeyEvent *e)
{
	clock_t start, end;

	switch (e->key())
	{
	case Qt::Key_E:								//显示所有轮廓

		cout << "checksil time: ";
		start = clock();

		glLoadIdentity();
		glScalef(myImage.scale, myImage.scale, myImage.scale);
		glRotatef(myImage.anglex3D, 1.0, 0.0, 0.0);
		glRotatef(myImage.angley3D, 0.0, 1.0, 0.0);
		glRotatef(myImage.anglez3D, 0.0, 0.0, 1.0);


		glGetDoublev(GL_MODELVIEW_MATRIX, myImage.modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, myImage.projMatrix);
		glGetIntegerv(GL_VIEWPORT, myImage.viewport);


		myImage.getViewVector();
		myImage.checkEdges();
		myImage.resetStroke();
		glutPostRedisplay();
		myImage.ready = false;

		end = clock();
		cout << (end - start) / 1000.0 << endl;


		cout << "Start to edit...\n";
		break;
	case Qt::Key_W:								//只显示contours

		draw_sc = false;
		draw_ridges = false;

		cout << "checksil time: ";
		start = clock();

		glLoadIdentity();
		glScalef(myImage.scale, myImage.scale, myImage.scale);
		glRotatef(myImage.anglex3D, 1.0, 0.0, 0.0);
		glRotatef(myImage.angley3D, 0.0, 1.0, 0.0);
		glRotatef(myImage.anglez3D, 0.0, 0.0, 1.0);


		glGetDoublev(GL_MODELVIEW_MATRIX, myImage.modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, myImage.projMatrix);
		glGetIntegerv(GL_VIEWPORT, myImage.viewport);


		myImage.getViewVector();
		myImage.checkEdges();
		myImage.resetStroke();
		glutPostRedisplay();
		myImage.ready = false;

		end = clock();
		cout << (end - start) / 1000.0 << endl;


		cout << "Start to edit...\n";
		break;
	case Qt::Key_F:
		glutPostRedisplay();
		break;

	case Qt::Key_Y:
		if (myImage.ready)
		{
			cout << "start findHandlePoint...\n";
			start = clock();
			myImage.findHandlePoint();

			cout << "found HANDLE potins: " << myImage.nHandle << endl;
			cout << "start findAncher...\n";
			myImage.findAncher();
			end = clock();
			cout << "\ttime: " << (end - start) / 1000.0 << endl;

			cout << "found HANDLE potins: " << myImage.nHandle << endl;
			cout << "start deform...\n";
			start = clock();
			myImage.changePos();
			end = clock();
			cout << "\ttime: " << (end - start) / 1000.0 << endl;


			myImage.checkEdges();

			glutPostRedisplay();
			//glutSetWindow(window3D);
			glutPostRedisplay();

			cout << "\n--------\n";

		}
		break;
	case Qt::Key_B:
		myImage.scale += 0.1;
		glLoadIdentity();
		glScalef(myImage.scale, myImage.scale, myImage.scale);
		glRotatef(myImage.anglex3D, 1.0, 0.0, 0.0);
		glRotatef(myImage.angley3D, 0.0, 1.0, 0.0);
		glRotatef(myImage.anglez3D, 0.0, 0.0, 1.0);

		glGetDoublev(GL_MODELVIEW_MATRIX, myImage.modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, myImage.projMatrix);
		glGetIntegerv(GL_VIEWPORT, myImage.viewport);

		glutPostRedisplay();
		break;
	case Qt::Key_S:
		myImage.scale -= 0.1;
		if (myImage.scale < 0)
			myImage.scale = 0;
		glLoadIdentity();
		glScalef(myImage.scale, myImage.scale, myImage.scale);
		glRotatef(myImage.anglex3D, 1.0, 0.0, 0.0);
		glRotatef(myImage.angley3D, 0.0, 1.0, 0.0);
		glRotatef(myImage.anglez3D, 0.0, 0.0, 1.0);

		glGetDoublev(GL_MODELVIEW_MATRIX, myImage.modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, myImage.projMatrix);
		glGetIntegerv(GL_VIEWPORT, myImage.viewport);

		glutPostRedisplay();
		break;

	case Qt::Key_C:
		myImage.colorSil = !myImage.colorSil;
		glutPostRedisplay();
		break;

	}
}

void Sketch2D::mousePressEvent(QMouseEvent *e)
{
	vector3df curPoint;
	vector2di curVertex;
	vector2df curVertex2f;

	curVertex.X = e->x();
	curVertex.Y = myImage.height - e->y();

	myImage.ready = false;
	myImage.resetStroke();

	glutPostRedisplay();
}

void Sketch2D::mouseMoveEvent(QMouseEvent *e)
{
	vector3df curPoint;
	vector2di curVertex;
	vector2df curVertex2f;

	curVertex.X = e->x();
	curVertex.Y = myImage.height - e->y();
	myImage.transToObjCood(curVertex.X, curVertex.Y, 0, curPoint.X, curPoint.Y, curPoint.Z);
	curVertex2f.X = curVertex.X;
	curVertex2f.Y = curVertex.Y;
	myImage.defStroke.push_back(curVertex2f);
	myImage.defStroke3D.push_back(curPoint);

	glutPostRedisplay();
}

void Sketch2D::mouseReleaseEvent(QMouseEvent *e)
{
	if (!myImage.defStroke.empty() && myImage.defStroke.size() > 1)
	{
		cout << "start matchRefStroke...\n";
		myImage.matchRefStroke();


		cout << "start reconStroke...\n";

		myImage.reconStroke();

		myImage.ready = true;
		glutPostRedisplay();
	}
}