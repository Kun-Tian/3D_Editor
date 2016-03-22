#include "gcodeviewer.h"
#include <QFileDialog>
#include "qmenubar.h"
#include <QLayout> 
Gcodeviewer::Gcodeviewer(QWidget *parent)
    : QMainWindow(parent)
{

   /* openAct = new QAction(QIcon("Images/folder.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));



    fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(openAct);

    fileToolBar = addToolBar(tr("&File"));

    fileToolBar->addAction(openAct);

    //connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	

     connect(openAct,SIGNAL(triggered()),this,SLOT(onBtLoadGCode()));
*/

	 QVBoxLayout *topLayout = new QVBoxLayout;

	 layerselect = new QSlider(Qt::Vertical, this);
	 layerselect->setMinimum(0); 
	 layerselect->setMaximum(100);
	 layerselect->setSingleStep(1);
	 layerselect->setPageStep(1);	 
	 connect(layerselect, SIGNAL(valueChanged(int)), this, SLOT(changeLayer()));
	 layerselect->setGeometry(15, 20,45, 560);
	 topLayout->addWidget(layerselect);

	QVBoxLayout *bomLayout = new QVBoxLayout;

	GLDock = new DockOpenGL(tr("Print preview"), this);

	GLDock->setMinimumWidth(400);
	GLDock->setGeometry(QRect(80, 10, 800, 580));
	bomLayout->addWidget(GLDock);
	//QVBoxLayout *mainLayout = new QVBoxLayout;
	//mainLayout->addLayout(topLayout);
	//mainLayout->addLayout(bomLayout);
	//this->setLayout(mainLayout);
     resize(900,600);
	 GLDock->loadGCode("..\\file\\path.gcode");
	 layerselect->setMinimum(0);
	 layerselect->setMaximum(GLDock->MaxLayer);
	 layerselect->setValue(GLDock->MaxLayer);

}

Gcodeviewer::~Gcodeviewer()
{
	delete GLDock;
}

void Gcodeviewer::onBtLoadGCode()
{
        QString qFileName=QFileDialog::getOpenFileName(this, "Select a G-Code to interprete", ".", "*.*");
        if(qFileName=="")
                return;
        string fileName=qFileName.toStdString();
        GLDock->loadGCode(fileName);
		layerselect->setMinimum(1);
		layerselect->setMaximum(GLDock->MaxLayer);
		layerselect->setValue(GLDock->MaxLayer);
		
}
/*
void Gcodeviewer::loadGCode(string fileName)
{
        GLDock->loadGCode(fileName);
		layerselect->setMaximum(GLDock->MaxLayer);
		layerselect->setValue(GLDock->MaxLayer);
}*/
void Gcodeviewer::setHeight(float h){
	layerheight = h;
	GLDock->setHeight(h);
}

void Gcodeviewer::changeLayer(){
	GLDock->changeLayer(layerselect->value());
}