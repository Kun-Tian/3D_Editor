#include "VertexItem.h"
#include <QtWidgets>
#include "qdebug.h"

MyMesh *VertexItem::mm;
float VertexItem::s = 300;

Vec<3U, float> VertexItem::x = Vec<3U, float>(0, 0, 0);
Vec<3U, float> VertexItem::y = Vec<3U, float>(0, 0, 0);
Vec<3U, float> VertexItem::z = Vec<3U, float>(0, 0, 0);
Vec<3U, float> VertexItem::rx = Vec<3U, float>(0, 0, 0);
Vec<3U, float> VertexItem::ry = Vec<3U, float>(0, 0, 0);
Vec<3U, float> VertexItem::rz = Vec<3U, float>(0, 0, 0);

Vec<3U, float> VertexItemX::x = Vec<3U, float>(0, 1, 0);
Vec<3U, float> VertexItemX::y = Vec<3U, float>(0, 0, 1);
Vec<3U, float> VertexItemX::z = Vec<3U, float>(1, 0, 0);
Vec<3U, float> VertexItemX::rx = Vec<3U, float>(0, 0, 1);
Vec<3U, float> VertexItemX::ry = Vec<3U, float>(1, 0, 0);
Vec<3U, float> VertexItemX::rz = Vec<3U, float>(0, 1, 0);

Vec<3U, float> VertexItemY::x = Vec<3U, float>(0, 0, 1);
Vec<3U, float> VertexItemY::y = Vec<3U, float>(1, 0, 0);
Vec<3U, float> VertexItemY::z = Vec<3U, float>(0, 1, 0);
Vec<3U, float> VertexItemY::rx = Vec<3U, float>(0, 1, 0);
Vec<3U, float> VertexItemY::ry = Vec<3U, float>(0, 0, 1);
Vec<3U, float> VertexItemY::rz = Vec<3U, float>(1, 0, 0);

Vec<3U, float> VertexItemZ::x = Vec<3U, float>(1, 0, 0);
Vec<3U, float> VertexItemZ::y = Vec<3U, float>(0, 1, 0);
Vec<3U, float> VertexItemZ::z = Vec<3U, float>(0, 0, 1);
Vec<3U, float> VertexItemZ::rx = Vec<3U, float>(1, 0, 0);
Vec<3U, float> VertexItemZ::ry = Vec<3U, float>(0, 1, 0);
Vec<3U, float> VertexItemZ::rz = Vec<3U, float>(0, 0, 1);

VertexItem::VertexItem()
{
	setFlags(ItemIsSelectable | ItemIsMovable);
	setAcceptHoverEvents(true);
	//s = 400;
}

VertexItem::~VertexItem()
{
}

QRectF VertexItem::boundingRect()const
{
	return QRectF(-3, -3, 7, 7);
}

QPainterPath VertexItem::shape()const
{
	QPainterPath path;
	path.addRect(QRectF(-3, -3, 7, 7));
	return path;
}

void VertexItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
	if (option->state & QStyle::State_Selected)
	{
		painter->setBrush(QColor(0xff707070));
		painter->setPen(QPen(QColor(0xffc03030), 1));
		painter->drawEllipse(-1, -1, 3, 3);
	}
	else
	{
		painter->setPen(QPen(QColor(0xff707070), 1));
		painter->drawEllipse(0, 0, 1, 1);
	}
}

//void VertexItem::setModel(MyMesh *m)
//{
//	mm = m;
//}

void VertexItem::setVertex(int vertex)
{
	v = vertex;
}

//void VertexItem::setDirection(char d)
//{
//	if (d == 'x' || d == 'X')
//	{
//		x = Vec<3U, float>(0, 1, 0);
//		y = Vec<3U, float>(0, 0, 1);
//		z = Vec<3U, float>(1, 0, 0);
//		rx = Vec<3U, float>(0, 0, 1);
//		ry = Vec<3U, float>(1, 0, 0);
//		rz = Vec<3U, float>(0, 1, 0);
//	}
//	else if (d == 'y' || d == 'Y')
//	{
//		x = Vec<3U, float>(0, 0, 1);
//		y = Vec<3U, float>(1, 0, 0);
//		z = Vec<3U, float>(0, 1, 0);
//		rx = Vec<3U, float>(0, 1, 0);
//		ry = Vec<3U, float>(0, 0, 1);
//		rz = Vec<3U, float>(1, 0, 0);
//	}
//	else if (d == 'z' || d == 'Z')
//	{
//		x = Vec<3U, float>(1, 0, 0);
//		y = Vec<3U, float>(0, 1, 0);
//		z = Vec<3U, float>(0, 0, 1);
//		rx = Vec<3U, float>(1, 0, 0);
//		ry = Vec<3U, float>(0, 1, 0);
//		rz = Vec<3U, float>(0, 0, 1);
//	}
//	else
//	{
//		x = Vec<3U, float>(0, 0, 0);
//		y = Vec<3U, float>(0, 0, 0);
//		z = Vec<3U, float>(0, 0, 0);
//		rx = Vec<3U, float>(0, 0, 0);
//		ry = Vec<3U, float>(0, 0, 0);
//		rz = Vec<3U, float>(0, 0, 0);
//	}
//}

void VertexItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	update();
}

void VertexItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	//if (event->modifiers() & Qt::ShiftModifier) {
	//	stuff << event->pos();
	//	update();
	//	return;
	//}
	QGraphicsItem::mouseMoveEvent(event);
}

void VertexItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	update();
}

void VertexItemX::updatePos()
{
	setX((x DOT mm->vertices[v]) * s);
	setY(-(y DOT mm->vertices[v]) * s);
	setZValue((z DOT mm->vertices[v]) * s);
}

Vec<3U, float> VertexItemX::getPos()
{
	Vec<3U, float> q,p(pos().rx()/s, -pos().ry()/s, zValue()/s);
	q= Vec<3U, float>(rx DOT p, ry DOT p, rz DOT p);
	return q;
}

void VertexItemY::updatePos()
{
	setX((x DOT mm->vertices[v]) * s);
	setY(-(y DOT mm->vertices[v]) * s);
	setZValue((z DOT mm->vertices[v]) * s);
}

Vec<3U, float> VertexItemY::getPos()
{
	Vec<3U, float> q, p(pos().rx() / s, -pos().ry() / s, zValue() / s);
	q = Vec<3U, float>(rx DOT p, ry DOT p, rz DOT p);
	return q;
}

void VertexItemZ::updatePos()
{
	setX((x DOT mm->vertices[v]) * s);
	setY(-(y DOT mm->vertices[v]) * s);
	setZValue((z DOT mm->vertices[v]) * s);
}

Vec<3U, float> VertexItemZ::getPos()
{
	Vec<3U, float> q, p(pos().rx() / s, -pos().ry() / s, zValue() / s);
	q = Vec<3U, float>(rx DOT p, ry DOT p, rz DOT p);
	return q;
}