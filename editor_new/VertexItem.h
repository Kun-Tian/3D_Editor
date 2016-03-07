#pragma once
#include "qgraphicsitem.h"
#include "qpainter.h"
#include "MyMesh.h"

using namespace trimesh;

class VertexItem :
	public QGraphicsItem
{
public:
	VertexItem();
	~VertexItem();
	//void setModel(MyMesh *m);
	void setVertex(int vertex);
	//void setDirection(char d);
	virtual void updatePos(){};
	virtual Vec<3U, float> getPos(){ return 0; };

	QRectF boundingRect() const Q_DECL_OVERRIDE;
	QPainterPath shape() const Q_DECL_OVERRIDE;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
public:
	static MyMesh *mm;
	int v;
	static float s;
	static Vec<3U, float> x, y, z, rx, ry, rz;
};

class VertexItemX :
	public VertexItem
{
public:
	VertexItemX() :VertexItem(){};
	void updatePos();
	Vec<3U, float> getPos();
	static Vec<3U, float> x, y, z, rx, ry, rz;
};

class VertexItemY :
	public VertexItem
{
public:
	VertexItemY() :VertexItem(){};
	void updatePos();
	Vec<3U, float> getPos();
	static Vec<3U, float> x, y, z, rx, ry, rz;
};

class VertexItemZ :
	public VertexItem
{
public:
	VertexItemZ() :VertexItem(){};
	void updatePos();
	Vec<3U, float> getPos();
	static Vec<3U, float> x, y, z, rx, ry, rz;
};