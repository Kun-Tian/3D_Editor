#include "Line.h"


Line::Line(double d1, double d2, double d3)
{
	a = d1; b = d2; c = d3;
}

Line::~Line(void)
{
}

// 根据已知两点坐标，求过这两点的直线解析方程： a*x+b*y+c = 0  (a >= 0)  
Line Line::makeline(vector2df p1, vector2df p2)
{
	Line tl;
	int sign = 1;
	tl.a = p2.Y - p1.Y;
	if (tl.a<0)
	{
		sign = -1;
		tl.a = sign*tl.a;
	}
	tl.b = sign*(p1.X - p2.X);
	tl.c = sign*(p1.Y*p2.X - p1.X*p2.Y);
	return tl;
}

// 如果两条直线 l1(a1*x+b1*y+c1 = 0), l2(a2*x+b2*y+c2 = 0)相交，返回true，且返回交点p  
bool Line::lineintersect(Line l2, vector2df &p) // 是 L1，L2 
{
	double d = a*l2.b - l2.a*b;
	if (abs(d)<ROUNDING_ERROR) // 不相交 
		return false;
	p.X = (l2.c*b - c*l2.b) / d;
	p.Y = (l2.a*c - a*l2.c) / d;
	return true;
}


/******************************************************************************
r=multiply(sp,ep,op),得到(sp-op)*(ep-op)的叉积
r>0:ep在矢量opsp的逆时针方向；
r=0：opspep三点共线；
r<0:ep在矢量opsp的顺时针方向
******************************************************************************/

double multiply(vector2df sp, vector2df ep, vector2df op)
{
	return((sp.X - op.X)*(ep.Y - op.Y) - (ep.X - op.X)*(sp.Y - op.Y));
}




// 如果线段u和v相交(包括相交在端点处)时，返回true 
bool intersect(line2d<double> u, line2d<double> v)
{
	return((max(u.start.X, u.end.X) >= min(v.start.X, v.end.X)) &&                     //排斥实验 
		(max(v.start.X, v.end.X) >= min(u.start.X, u.end.X)) &&
		(max(u.start.Y, u.end.Y) >= min(v.start.Y, v.end.Y)) &&
		(max(v.start.Y, v.end.Y) >= min(u.start.Y, u.end.Y)) &&
		(multiply(v.start, u.end, u.start)*multiply(u.end, v.end, u.start) >= 0) &&         //跨立实验 
		(multiply(u.start, v.end, v.start)*multiply(v.end, u.end, v.start) >= 0));
}

/* 判断点p是否在线段l上，条件：(p在线段l所在的直线上)&& (点p在以线段l为对角线的矩形内) */
bool online(line2d<double> l, vector2df p)
{
	return((multiply(l.end, p, l.start) == 0)
		&& (((p.X - l.start.X)*(p.X - l.end.X) <= 0) && ((p.Y - l.start.Y)*(p.Y - l.end.Y) <= 0)));
}


// 如果线段l1和l2相交(前提是必须相交)，返回true且交点由(inter)返回，否则返回false 
bool intersection(line2d<double> l1, line2d<double> l2, vector2df &inter)
{
	Line ll1, ll2;
	ll1 = ll1.makeline(l1.start, l1.end);
	ll2 = ll2.makeline(l2.start, l2.end);
	if (ll1.lineintersect(ll2, inter))
	{
		return online(l1, inter);
	}
	else
		return false;
}




