#include "SketchWidget.h"


SketchWidget::SketchWidget()
{
	QGridLayout *layout = new QGridLayout();
	sketch2D = new Sketch2D();
	sketch3D = new Sketch3D();

	QVBoxLayout *vLayout = new QVBoxLayout();

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
