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

	SketchWidget *sketchWidget = new SketchWidget();
	tab->addTab(sketchWidget, "Sketch");

	QWidget *printWidget = new QWidget();
	QGridLayout *printLayout = new QGridLayout();
	printSetting = new QPushButton("Setting");
	printPrint = new QPushButton("Print");
	printLayout->addWidget(printSetting, 0, 0);
	printLayout->addWidget(printPrint, 1, 0);
	printLayout->setColumnStretch(1, 1);
	printLayout->setRowStretch(2, 1);
	printWidget->setLayout(printLayout);
	tab->addTab(printWidget, "Print");

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(tab);
	setLayout(layout);


	connect(printSetting, SIGNAL(clicked()), this, SLOT(open_setting()));
	connect(printPrint, SIGNAL(clicked()), this, SLOT(begin_print()));

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