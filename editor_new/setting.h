#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include "ui_setting.h"
#include <qdebug.h>

class setting : public QWidget
{
	Q_OBJECT

public:
	setting(QWidget *parent = 0);
	~setting();
	void read();
private:
	Ui::setting ui;
	
	public slots:
	void save();
	void cancel();
};

#endif // SETTING_H
