#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "drawing.h"
#include "Sketch2D.h"
#include "Sketch3D.h"


class SketchWidget :
	public QWidget
{
public:
	SketchWidget();
	~SketchWidget();

	Sketch2D *sketch2D;
	Sketch3D *sketch3D;

	QPushButton *editButton, *confirmButton;
};

