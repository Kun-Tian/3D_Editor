#include "EditWidget.h"
#include <QPixmap>
#include <QImage>
#include <fstream>

#define MAX_FACES 50000

using namespace std;

EditWidget::EditWidget()
{
	loadFromFile = true;
	//mm = new MyMesh("1.off");
	//VertexItem::mm = mm;
	//mm->need_bsphere();
	//for (int i = 0; i < mm->vertices.size(); i++)
	//	mm->vertices[i] = (mm->vertices[i] - mm->bsphere.center) / mm->bsphere.r;
	QGridLayout *layout = new QGridLayout();

	viewX = new View2D("Top left view");
	//viewX->mm = mm;
	sceneX = new QGraphicsScene();
	//for (int i = 0; i < mm->vertices.size(); i++)
	//{
	//	VertexItemX *v = new VertexItemX();
	//	v->setVertex(i);
	//	v->updatePos();
	//	sceneX->addItem(v);
	//	viewX->vertexList.push_back(v);
	//}
	viewX->view()->setScene(sceneX);

	viewY = new View2D("Top right view");
	//viewY->mm = mm;
	sceneY = new QGraphicsScene();
	//for (int i = 0; i < mm->vertices.size(); i++)
	//{
	//	VertexItemY *v = new VertexItemY();
	//	v->setVertex(i);
	//	v->updatePos();
	//	sceneY->addItem(v);
	//	viewY->vertexList.push_back(v);
	//}
	viewY->view()->setScene(sceneY);

	viewZ = new View2D("Bottom left view");
	//viewZ->mm = mm;
	sceneZ = new QGraphicsScene();
	//for (int i = 0; i < mm->vertices.size(); i++)
	//{
	//	VertexItemZ *v = new VertexItemZ();
	//	v->setVertex(i);
	//	v->updatePos();
	//	sceneZ->addItem(v);
	//	viewZ->vertexList.push_back(v);
	//}
	viewZ->view()->setScene(sceneZ);

	view3D = new View3D();
	//view3D->setModel(mm);

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
	saveAsButton = new QPushButton(tr("Save as"));
	saveButton = new QPushButton(tr("Save"));
	normButton = new QPushButton(tr("Norm"));
	deleteButton = new QPushButton(tr("Delete"));
	sketchButton = new QPushButton(tr("Sketch"));
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

	//MyMesh *m = new MyMesh("mesh.stl");
	//m->write("mesh.smf");
	//return;

	if (loadFromFile)
	{
		QString path = QFileDialog::getOpenFileName(this, tr("Open 3D Object"), ".", tr("3D Object Files(*.ply *.ray *.obj *.off *.sm *.smf *.stl *.dae)"));
		if (path.length() == 0)
			return;
		saveName = path;
		mm = new MyMesh(path.toStdString().c_str());
		if (mm->faces.size() > MAX_FACES)
		{
			mm->write("temp.smf");
			system("QSlim.exe -o temp.smf -t 50000 temp.smf");
			delete mm;
			mm = new MyMesh("temp.smf");
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
	int maxVal = mm->vertices.size();
	progress->setRange(0, maxVal);
	int val = 0;
	int d = 0;
	for (int i = 0; i < mm->vertices.size(); i++)
	{
		progress->setValue(val);
		val++;
		if (mm->deletedV[i])
		{
			d++;
			mm->vertices.erase(mm->vertices.begin() + i);
			mm->deletedV.erase(mm->deletedV.begin() + i);
			for (int j = 0; j < mm->faces.size(); j++)
			{
				if (mm->faces[j][0] == i || mm->faces[j][1] == i || mm->faces[j][2] == i)
				{
					mm->faces.erase(mm->faces.begin() + j);
					j--;
				}
				else
				{
					for (int k = 0; k < 3; k++)
						if (mm->faces[j][k]>i)
							mm->faces[j][k]--;
				}
			}
			i--;
		}
	}
	mm->selected.clear();
	mm->selected.resize(mm->vertices.size(),false);
	mm->deletedF.clear();
	mm->deletedF.resize(mm->faces.size(),false);

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
 