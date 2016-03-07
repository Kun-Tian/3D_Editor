#include "setting.h"
#include "profile.h"
#include <QTextCodec>
setting::setting(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(save()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	
	ui.LayerHeight->setRange(0, 0.4);
	ui.LayerHeight->setSingleStep(0.05);
	ui.LayerHeight->setDecimals(2);
	ui.LayerHeight->setSuffix("mm");
	ui.Infill->setRange(0, 100);
	ui.Infill->setSingleStep(1);
	ui.Infill->setSuffix("%");
	ui.NumofShells->setRange(0, 100);
	ui.NumofShells->setSingleStep(1);
	ui.Temperature->setRange(0, 280);
	ui.Temperature->setSingleStep(1);
	ui.ModelHeight->setRange(20, 150);
	ui.ModelHeight->setSingleStep(1);
	ui.ModelHeight->setSuffix("mm");
	char *string = "¡ãC";
	QTextCodec* gbk_codec = QTextCodec::codecForName("GBK");
	QString gbk_string = gbk_codec->toUnicode(string); 
	ui.Temperature->setSuffix(gbk_string);
	/*
	ui.Quality->setCurrentIndex(1);
	ui.Raft->setChecked(1);
	ui.Support->setChecked(0);
	ui.LayerHeight->setValue(0.10);
	ui.Infill->setValue(10);
	ui.NumofShells->setValue(2);
	ui.Temperature->setValue(230);
	ui.ModelHeight->setValue(20);*/
}

setting::~setting()
{

}

char* Qs2c(QString s){
	char*  buf;
	QByteArray *ba = new QByteArray;
	*ba = s.toLatin1();
	buf = ba->data();
	return buf;
}

void setting::read(){
	profile *p = read_settings("..\\file\\config.ini");
	ui.Quality->setCurrentIndex(p->Quality);
	ui.Raft->setChecked(p->Raft);
	ui.Support->setChecked(p->Support);
	ui.LayerHeight->setValue(atof(p->LayerHeight));
	ui.Infill->setValue(atoi(p->Infill));
	ui.NumofShells->setValue(atoi(p->NumofShells));
	ui.Temperature->setValue(atoi(p->Temperature));
	ui.ModelHeight->setValue(atoi(p->ModelHeight));
}

void setting::cancel(){
	this->close();
}

void setting::save(){
	FILE *f;
	fopen_s(&f, "..\\file\\config.ini", "w");
	double d;
	int i;
	QString n("\n");
	QString yes("T");
	QString no("F");
	QString Q("Quality=");
	QString R("Raft=");
	QString S("Support=");
	QString L("LayerHeight=");
	QString I("Infill=");
	QString N("NumofShells=");
	QString T("Temperature=");
	QString M("ModelHeight=");

	i = ui.Quality->currentIndex()+1;
	QString s = QString::number(i, 10);
	Q = Q + s + n;
	
	if (ui.Raft->isChecked())R = R + yes + n;
	else R = R + no + n;

	if (ui.Support->isChecked())S = S + yes + n;
	else S = S + no + n;

	d = ui.LayerHeight->value();
	L = L + QString::number(d,'g',3) + n;

	i = ui.Infill->value();
	I = I + QString::number(i,10) + n;

	i = ui.NumofShells->value();
	N = N + QString::number(i, 10) + n;

	i = ui.Temperature->value();
	T = T + QString::number(i, 10) + n;

	i = ui.ModelHeight->value();
	M = M + QString::number(i, 10) + n;


	QString result = Q + R + S + L + I + N + T + M;
	fwrite(Qs2c(result), result.length(), 1, f);
	fclose(f);
	this->close();

}

