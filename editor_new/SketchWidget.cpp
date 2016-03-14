#include "SketchWidget.h"

extern Image myImage;

SketchWidget::SketchWidget()
{
	myImage.init();

	QGridLayout *layout = new QGridLayout();
	sketch2D = new Sketch2D();
	sketch3D = new Sketch3D();

	QVBoxLayout *vLayout = new QVBoxLayout();
	editButton = new QPushButton(tr("Edit"));
	confirmButton = new QPushButton(tr("Confirm"));
	vLayout->addWidget(editButton);
	vLayout->addWidget(confirmButton);
	vLayout->addStretch();

	layout->addLayout(vLayout, 0, 0, 1, 1);
	layout->addWidget(sketch2D, 0, 1, 1, 1);
	layout->addWidget(sketch3D, 0, 2, 1, 1);
	layout->setColumnStretch(1, 1);
	layout->setColumnStretch(2, 1);

	setLayout(layout);
}


SketchWidget::~SketchWidget()
{
}
