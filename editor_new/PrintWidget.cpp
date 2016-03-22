#include "PrintWidget.h"
#include <QPixmap>
#include <QImage>
#include <fstream>
#include "QSlim.h"
#define MAX_FACES 50000


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
			m->vertices.push_back(trimesh::point(data[0], data[1], data[2]));
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

PrintWidget::PrintWidget()
{
	loadFromFile = true;

	view3D = new View3D_();


	QGridLayout *printLayout = new QGridLayout();
	QVBoxLayout *vLayout = new QVBoxLayout();

	printSetting = new QPushButton("Setting");
	printPrint = new QPushButton("Print");
	printPreview = new QPushButton("Preview");
	rotate = new QPushButton("Rotate");
	view = new QPushButton("View");
	save_button = new QPushButton("Save");
	
	vLayout->addWidget(rotate);
	vLayout->addWidget(view);
	vLayout->addWidget(save_button);
	vLayout->addSpacing(20);
	vLayout->addWidget(printSetting);
	vLayout->addWidget(printPreview);
	vLayout->addWidget(printPrint);
	vLayout->addStretch();
	//printLayout->addWidget(printSetting, 0, 0);
	//printLayout->addWidget(printPreview, 1, 0);
	//printLayout->addWidget(printPrint, 2, 0);
	//printLayout->addWidget(rolate, 3, 0);
	//printLayout->addWidget(view, 4, 0);
	//printLayout->setColumnStretch(1, 1);
	//printLayout->setRowStretch(5, 1);
	printLayout->addLayout(vLayout, 0, 0, 2, 1);
	printLayout->addWidget(view3D, 0, 1);
	printLayout->setColumnStretch(0, 1);
	printLayout->setColumnStretch(1, 5);
	//printLayout->setColumnStretch(1, 1);
	printLayout->setRowStretch(0, 1);
	//printLayout->setRowStretch(1, 1);
	//printLayout->setRowStretch(2, 5);
	//printLayout->setRowStretch(6, 1);
	
	setLayout(printLayout);

	connect(printSetting, SIGNAL(clicked()), this, SLOT(open_setting()));
	connect(printPrint, SIGNAL(clicked()), this, SLOT(begin_print()));
	connect(printPreview, SIGNAL(clicked()), this, SLOT(print_preview()));
	connect(rotate, SIGNAL(clicked()), this, SLOT(rolate_mode()));
	connect(view, SIGNAL(clicked()), this, SLOT(view_mode()));
	connect(save_button, SIGNAL(clicked()), this, SLOT(save()));
	init();
}


void PrintWidget::init()
{
	if (0)//loadFromFile)
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



	int maxVal = mm->vertices.size();

	VertexItem::mm = mm;
	mm->need_bsphere();
	for (int i = 0; i < mm->vertices.size(); i++)
		mm->vertices[i] = (mm->vertices[i] - mm->bsphere.center) / mm->bsphere.r;



	view3D->setModel(mm);

	view3D->updateGL();


}



void PrintWidget::open_setting(){
	setting *s = new setting;
	s->show();
	s->read();
}

void PrintWidget::begin_print(){
	char *offpath = "temp.off";// D:\\work\\database\\SHREC14LSGTB\\M000003.off";
	char *stlpath = "..\\file\\test.stl";
	off2stl(offpath, stlpath);

	CClientNet client;
	//连接到127.0.0.1（即本地）,端口号为8888的服务端  
	printf("Connect\n");
	if (client.Connect(8888, "172.28.37.201") == 0){

		if (client.request() == 0){
			QMessageBox::about(NULL, "warning", "printer is busy");
			client.Close();
			return;
		}

		char object_msg[20] = "object";
		client.SendMsg(object_msg, sizeof((object_msg)));
		Sleep(1000);
		client.SendFile("..\\file\\test.stl");
		printf("send stl file -------------------\n");

		char configuration_msg[20] = "configuration";
		client.SendMsg(configuration_msg, sizeof((configuration_msg)));
		Sleep(1000);
		client.SendFile("..\\file\\config.ini");
		printf("send config file -------------------\n");
		Sleep(1000);

		client.Close();
	}
	else QMessageBox::about(NULL, "warning", "can not connect to the printer");
}

void PrintWidget::print_preview(){
	char *offpath = "temp.off";// D:\\work\\database\\SHREC14LSGTB\\M000003.off";
	char *stlpath = "..\\file\\test.stl";
	off2stl(offpath, stlpath);
	float h = slide();
	w = new Gcodeviewer();
	w->setHeight(h);
	w->setWindowTitle("Print Preview");
	w->show();
}

float PrintWidget::slide(){
	profile *p = read_settings("..\\file\\config.ini");
	/*
	ui.Quality->setCurrentIndex(p->Quality);
	ui.Raft->setChecked(p->Raft);
	ui.Support->setChecked(p->Support);
	ui.LayerHeight->setValue(atof(p->LayerHeight));
	ui.Infill->setValue(atoi(p->Infill));
	ui.NumofShells->setValue(atoi(p->NumofShells));
	ui.Temperature->setValue(atoi(p->Temperature));
	ui.ModelHeight->setValue(atoi(p->ModelHeight));
	*/
	float scale = 30 / getheight("temp.off");
	QString Slic("..\\file\\test.stl --output ..\\file\\path.gcode");
	QString R(" --raft-layers 3"); //true
	QString S(" --support-material "); //true
	QString L(" --layer-height "); // char*
	QString H(p->LayerHeight);		// 
	QString I(" --fill-density ");  // char * X%
	QString D(p->Infill);
	QString Percent("%");
	QString Scale(" --scale ");
	QString act_s = QString::number(scale, 'g');

	QString result = Slic;
	if (p->Raft){
		result = result + R;
	}
	if (p->Support){
		result = result + S;
	}
	result = result + L + H + I + D + Percent + Scale + act_s;//;


	LPCWSTR strVariable2 = (const wchar_t*)result.utf16();

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = L"..\\slic3r\\slic3r-console.exe";
	//ShExecInfo.lpParameters = "..\\slic3r\\3.stl --output ..\\file\\test.gcode";
	ShExecInfo.lpParameters = strVariable2;//L"..\\slic3r\\test.stl --output ..\\file\\path.gcode --layer-height 0.2 --fill-density 15% ";
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	return atof(p->LayerHeight);
}

void PrintWidget::save()
{
	mm->write("temp.off");
}


PrintWidget::~PrintWidget()
{
}
