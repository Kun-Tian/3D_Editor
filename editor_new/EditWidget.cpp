#include "EditWidget.h"
#include <QPixmap>
#include <QImage>
#include <fstream>
#include "QSlim.h"

#define MAX_FACES 50000

using namespace std;


static void qslim_init()
{
	int i;
	qDebug() << "Reading input ..." << endl;
	qDebug() << "Cleaning up initial input ..." << endl;
	int initialVertCount = M0.vertCount();
	int initialEdgeCount = M0.edgeCount();
	int initialFaceCount = M0.faceCount();
	for (i = 0; i<M0.faceCount(); i++)
		if (!M0.face(i)->plane().isValid())
			M0.killFace(M0.face(i));
	M0.removeDegeneracy(M0.allFaces());
	for (i = 0; i<M0.vertCount(); i++)
	{
		if (M0.vertex(i)->edgeUses().length() == 0)
			M0.vertex(i)->kill();
	}
	qDebug() << "Input model summary:" << endl;
	qDebug() << "    Vertices    : " << initialVertCount << endl;
	qDebug() << "    Edges       : " << initialEdgeCount << endl;
	int man = 0, non = 0, bndry = 0, bogus = 0;
	for (i = 0; i<M0.edgeCount(); i++)
		switch (M0.edge(i)->faceUses().length())
	{
		case 0:
			bogus++;
			break;
		case 1:
			bndry++;
			break;
		case 2:
			man++;
			break;
		default:
			non++;
			break;
	}
	if (bogus)
		qDebug() << "        Bogus       : " << bogus << endl;
	qDebug() << "        Boundary    : " << bndry << endl;
	qDebug() << "        Manifold    : " << man << endl;
	qDebug() << "        Nonmanifold : " << non << endl;

	qDebug() << "    Faces       : " << initialFaceCount << endl;
}
static void qslim_run()
{
	decimate_init(M0, pair_selection_tolerance);
	while (M0.validFaceCount > face_target&& decimate_min_error() < error_tolerance)
		decimate_contract(M0);
	qDebug() << "Done!" << endl;
}
static void InitM0(MyMesh *m)
{
	for (size_t i = 0; i<m->vertices.size(); i++)
	{
		Vec3 v(m->vertices[i][0], m->vertices[i][1], m->vertices[i][2]);
		M0.in_Vertex(v);
	}
	for (size_t i = 0; i<m->faces.size(); i++)
	{
		M0.in_Face(m->faces[i][0], m->faces[i][1], m->faces[i][2]);
	}
}
static void ReplaceM(MyMesh *m)
{
	m->vertices.clear();
	m->faces.clear();
	int* map = new int[M0.vertCount()];
	for (int i = 0; i<M0.vertCount(); i++)
		map[i] = -1;
	for (int i = 0; i<M0.vertCount(); i++)
	{
		if (M0.vertex(i)->isValid())
		{
			map[i] = m->vertices.size();
			real* data = M0.vertex(i)->raw();
			m->vertices.push_back(trimesh::point(data[0],data[1],data[2]));
		}
	}
	for (int i = 0; i<M0.faceCount(); i++)
	{
		if (M0.face(i)->isValid())
		{
			Vertex* v0 = M0.face(i)->vertex(0);
			Vertex* v1 = M0.face(i)->vertex(1);
			Vertex* v2 = M0.face(i)->vertex(2);
			m->faces.push_back(TriMesh::Face(map[v0->uniqID], map[v1->uniqID], map[v2->uniqID]));
		}
	}
	delete[] map;
}




EditWidget::EditWidget()
{
	loadFromFile = true;
	QGridLayout *layout = new QGridLayout();

	viewX = new View2D("Top left view");
	sceneX = new QGraphicsScene();
	viewX->view()->setScene(sceneX);

	viewY = new View2D("Top right view");
	sceneY = new QGraphicsScene();
	viewY->view()->setScene(sceneY);

	viewZ = new View2D("Bottom left view");
	sceneZ = new QGraphicsScene();
	viewZ->view()->setScene(sceneZ);

	view3D = new View3D();

	viewX->v1 = viewY;
	viewX->v2 = viewZ;
	viewX->v3 = view3D;
	viewY->v1 = viewX;
	viewY->v2 = viewZ;
	viewY->v3 = view3D;
	viewZ->v1 = viewX;
	viewZ->v2 = viewY;
	viewZ->v3 = view3D;

	QVBoxLayout *vLayout = new QVBoxLayout();
	openButton = new QPushButton(tr("Load"));
	saveAsButton = new QPushButton(tr("Save As"));
	saveButton = new QPushButton(tr("Save"));
	normButton = new QPushButton(tr("Normalize"));
	deleteButton = new QPushButton(tr("Delete"));
	sketchButton = new QPushButton(tr("Sketch Edit"));
	status = new QLabel();
	progress = new QProgressBar();
	vLayout->addWidget(openButton);
	vLayout->addWidget(saveAsButton);
	vLayout->addWidget(saveButton);
	vLayout->addSpacing(20);
	vLayout->addWidget(normButton);
	vLayout->addWidget(deleteButton);
	vLayout->addWidget(sketchButton);
	vLayout->addStretch();
	vLayout->addWidget(status);
	vLayout->addWidget(progress);

	layout->addLayout(vLayout, 0, 0, 2, 1);
	layout->addWidget(viewX, 0, 1);
	layout->addWidget(viewY, 0, 2);
	layout->addWidget(viewZ, 1, 1);
	layout->addWidget(view3D, 1, 2);
	layout->setColumnStretch(1, 1);
	layout->setColumnStretch(2, 1);
	layout->setRowStretch(0, 1);
	layout->setRowStretch(1, 1);

	setLayout(layout);
	connect(openButton, SIGNAL(clicked()), this, SLOT(load()));
	connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
	connect(saveButton, SIGNAL(clicked()), this, SLOT(saveToFile()));
	connect(normButton, SIGNAL(clicked()), this, SLOT(norm()));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteVertex()));
	connect(sketchButton, SIGNAL(clicked()), this, SLOT(sketch()));
}

void EditWidget::load()
{
	loadFromFile = true;
	init();
}

void EditWidget::init()
{
	if (loadFromFile)
	{
		QString path = QFileDialog::getOpenFileName(this, tr("Open 3D Object"), ".", tr("3D Object Files(*.ply *.ray *.obj *.off *.sm *.smf *.stl *.dae)"));
		if (path.length() == 0)
			return;
		saveName = path;
		mm = new MyMesh(path.toStdString().c_str());
		if (mm->faces.size() > MAX_FACES)
		{
			InitM0(mm);
			qslim_init();
			face_target = MAX_FACES;
			error_tolerance = HUGE;
			will_use_plane_constraint = true;
			will_use_vertex_constraint = false;
			will_preserve_boundaries = true;
			will_preserve_mesh_quality = true;
			will_constrain_boundaries = true;
			boundary_constraint_weight = 1.0;
			will_weight_by_area = false;
			placement_policy = 1;
			pair_selection_tolerance = 0.0;
			qslim_run();
			ReplaceM(mm);
			mm->write("temp.off");
		}
		loadFromFile = false;
	}
	else
		mm = new MyMesh("temp.off");

	status->setText(tr("Processing..."));

	int maxVal = mm->vertices.size();
	progress->setRange(0, maxVal);

	VertexItem::mm = mm;
	mm->need_bsphere();
	for (int i = 0; i < mm->vertices.size(); i++)
		mm->vertices[i] = (mm->vertices[i] - mm->bsphere.center) / mm->bsphere.r;

	viewX->mm = mm;
	for (int i = 0; i < viewX->vertexList.size(); i++)
		delete viewX->vertexList[i];
	viewX->vertexList.clear();

	viewY->mm = mm;
	for (int i = 0; i < viewY->vertexList.size(); i++)
		delete viewY->vertexList[i];
	viewY->vertexList.clear();

	viewZ->mm = mm;
	for (int i = 0; i < viewZ->vertexList.size(); i++)
		delete viewZ->vertexList[i];
	viewZ->vertexList.clear();


	for (int i = 0; i < mm->vertices.size(); i++)
	{
		progress->setValue(i);
		VertexItemX *vx = new VertexItemX();
		vx->setVertex(i);
		vx->updatePos();
		sceneX->addItem(vx);
		viewX->vertexList.push_back(vx);

		VertexItemY *vy = new VertexItemY();
		vy->setVertex(i);
		vy->updatePos();
		sceneY->addItem(vy);
		viewY->vertexList.push_back(vy);

		VertexItemZ *vz = new VertexItemZ();
		vz->setVertex(i);
		vz->updatePos();
		sceneZ->addItem(vz);
		viewZ->vertexList.push_back(vz);
	}

	view3D->setModel(mm);

	viewX->update();
	viewY->update();
	viewZ->update();
	view3D->updateGL();

	progress->setValue(maxVal);
	status->setText(tr("Done."));
}

void EditWidget::saveAs()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Save 3D Object"), saveName, tr("3D Object Files(*.ply *.ray *.obj *.off *.sm *.smf *.stl *.dae)"));
	if (path.length() == 0)
		return;
	saveName = path;
	save();
	mm->write(saveName.toStdString().c_str());
}

void EditWidget::saveToFile()
{
	save();
	mm->write(saveName.toStdString().c_str());
}

void EditWidget::save()
{
	status->setText(tr("Processing..."));
	int maxVal = mm->vertices.size() + mm->faces.size();
	progress->setRange(0, maxVal);
	int d = 0;

	int newV = 0;
	int newF = 0;
	int* map = new int[mm->vertices.size()];
	for (int i = 0; i<mm->vertices.size(); i++)
		map[i] = -1;
	for (int i = 0; i<mm->vertices.size(); i++)
	{
		progress->setValue(d++);
		if (!mm->deletedV[i])
		{
			map[i] = newV;
			mm->vertices[newV] = mm->vertices[i];
			newV++;
		}
	}
	for (int i = 0; i<mm->faces.size(); i++)
	{
		progress->setValue(d++);
		if (!mm->deletedF[i])
		{
			mm->faces[newF][0] = map[mm->faces[i][0]];
			mm->faces[newF][1] = map[mm->faces[i][1]];
			mm->faces[newF][2] = map[mm->faces[i][2]];
			newF++;
		}
	}
	delete[] map;

	mm->vertices.resize(newV);
	mm->faces.resize(newF);
	mm->selected.clear();
	mm->selected.resize(newV, false);
	mm->deletedV.clear();
	mm->deletedV.resize(newV, false);
	mm->deletedF.clear();
	mm->deletedF.resize(newF, false);

	for (int i = mm->vertices.size(); i < viewX->vertexList.size(); i++)
	{
		delete viewX->vertexList[i];
		delete viewY->vertexList[i];
		delete viewZ->vertexList[i];
	}
	viewX->vertexList.resize(mm->vertices.size());
	viewY->vertexList.resize(mm->vertices.size());
	viewZ->vertexList.resize(mm->vertices.size());
	for (int i = 0; i < mm->vertices.size(); i++)
	{
		viewX->vertexList[i]->setVisible(true);
		viewY->vertexList[i]->setVisible(true);
		viewZ->vertexList[i]->setVisible(true);
	}

	viewX->updateSelection();
	viewY->updateSelection();
	viewZ->updateSelection();
	viewX->updatePos();
	viewY->updatePos();
	viewZ->updatePos();
	viewX->update();
	viewY->update();
	viewZ->update();
	view3D->updateGL();

	mm->write("temp.off");

	progress->setValue(maxVal);
	status->setText(tr("Done."));
}

void EditWidget::deleteVertex()
{
	status->setText(tr("Processing..."));
	int maxVal = mm->vertices.size();
	progress->setRange(0, maxVal);

	point v = point(0, 0, 0);
	for (int i = 0; i < mm->vertices.size(); i++)
	{
		if (!mm->deletedV[i])
		{
			v = mm->vertices[i];
			break;
		}
	}
	for (int i = 0; i < mm->vertices.size(); i++)
	{
		progress->setValue(i);
		if (mm->deletedV[i])
		{
			mm->vertices[i] = v;
		}
		if (mm->selected[i])
		{
			mm->selected[i] = false;
			mm->deletedV[i] = true;
			mm->vertices[i] = v;
			for (int j = 0; j < mm->faces.size(); j++)
			{
				if (mm->faces[j][0] == i || mm->faces[j][1] == i || mm->faces[j][2] == i)
				{
					mm->deletedF[j] = true;
				}
			}
			viewX->vertexList[i]->setSelected(false);
			viewX->vertexList[i]->setVisible(false);
			viewY->vertexList[i]->setSelected(false);
			viewY->vertexList[i]->setVisible(false);
			viewZ->vertexList[i]->setSelected(false);
			viewZ->vertexList[i]->setVisible(false);
		}
	}
	viewX->update();
	viewY->update();
	viewZ->update();
	view3D->updateGL();

	progress->setValue(maxVal);
	status->setText(tr("Done."));
}

void EditWidget::norm()
{
	status->setText(tr("Processing..."));
	int maxVal = mm->vertices.size();
	progress->setRange(0, maxVal);

	mm->bsphere.valid = false;
	mm->need_bsphere();
	for (int i = 0; i < mm->vertices.size(); i++)
	{
		progress->setValue(i);
		mm->vertices[i] = (mm->vertices[i] - mm->bsphere.center) / mm->bsphere.r;
	}
	viewX->updatePos();
	viewY->updatePos();
	viewZ->updatePos();
	viewX->update();
	viewY->update();
	viewZ->update();
	view3D->updateGL();

	progress->setValue(maxVal);
	status->setText(tr("Done."));
}

void EditWidget::sketch()
{
	save();
	system("SketchContour.exe");
	init();
}

EditWidget::~EditWidget()
{
}
 