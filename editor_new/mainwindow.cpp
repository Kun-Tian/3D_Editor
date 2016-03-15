#include "mainwindow.h"
#include <QHBoxLayout>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
	tab = new QTabWidget();

	QWidget *scanWidget = new QWidget();
	tab->addTab(scanWidget, "Scan");

	EditWidget *editWidget = new EditWidget();
	tab->addTab(editWidget, "Edit");

	QWidget *printWidget = new QWidget();
	QGridLayout *printLayout = new QGridLayout();
	printSetting = new QPushButton("Setting");
	printPrint = new QPushButton("Print");
	printPreview = new QPushButton("Preview");
	printLayout->addWidget(printSetting, 0, 0);
	printLayout->addWidget(printPreview, 1, 0);
	printLayout->addWidget(printPrint, 2, 0);
	printLayout->setColumnStretch(1, 1);
	printLayout->setRowStretch(3, 1);
	printWidget->setLayout(printLayout);
	tab->addTab(printWidget, "Print");

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(tab);
	setLayout(layout);
	

	connect(printSetting, SIGNAL(clicked()), this, SLOT(open_setting()));
	connect(printPrint, SIGNAL(clicked()), this, SLOT(begin_print()));
	connect(printPreview, SIGNAL(clicked()), this, SLOT(print_preview()));
}
void MainWindow::open_setting(){
	setting *s = new setting;
	s->show();
	s->read();
}

void MainWindow::begin_print(){
	char *offpath = "..\\shape\\temp.off";// D:\\work\\database\\SHREC14LSGTB\\M000003.off";
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

void MainWindow::print_preview(){
	char *offpath = "temp.off";// D:\\work\\database\\SHREC14LSGTB\\M000003.off";
	char *stlpath = "..\\file\\test.stl";
	off2stl(offpath, stlpath);
	slide();
	w = new Gcodeviewer;
	w->setWindowTitle("Print Preview");
	w->show();
}

void MainWindow::slide(){
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
	float scale = 30 / getheight("..\\shape\\temp.off");
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
	result = result + L + H + I + D + Percent + Scale +act_s;//;
	
	
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
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
}

