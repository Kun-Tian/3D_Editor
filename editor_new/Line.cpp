#include "Line.h"


Line::Line(double d1, double d2, double d3)
{
	a = d1; b = d2; c = d3;
}

Line::~Line(void)
{
}

// ������֪�������꣬����������ֱ�߽������̣� a*x+b*y+c = 0  (a >= 0)  
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

// �������ֱ�� l1(a1*x+b1*y+c1 = 0), l2(a2*x+b2*y+c2 = 0)�ཻ������true���ҷ��ؽ���p  
bool Line::lineintersect(Line l2, vector2df &p) // �� L1��L2 
{
	double d = a*l2.b - l2.a*b;
	if (abs(d)<ROUNDING_ERROR) // ���ཻ 
		return false;
	p.X = (l2.c*b - c*l2.b) / d;
	p.Y = (l2.a*c - a*l2.c) / d;
	return true;
}


/******************************************************************************
r=multiply(sp,ep,op),�õ�(sp-op)*(ep-op)�Ĳ��
r>0:ep��ʸ��opsp����ʱ�뷽��
r=0��opspep���㹲�ߣ�
r<0:ep��ʸ��opsp��˳ʱ�뷽��
******************************************************************************/

double multiply(vector2df sp, vector2df ep, vector2df op)
{
	return((sp.X - op.X)*(ep.Y - op.Y) - (ep.X - op.X)*(sp.Y - op.Y));
}




// ����߶�u��v�ཻ(�����ཻ�ڶ˵㴦)ʱ������true 
bool intersect(line2d<double> u, line2d<double> v)
{
	return((max(u.start.X, u.end.X) >= min(v.start.X, v.end.X)) &&                     //�ų�ʵ�� 
		(max(v.start.X, v.end.X) >= min(u.start.X, u.end.X)) &&
		(max(u.start.Y, u.end.Y) >= min(v.start.Y, v.end.Y)) &&
		(max(v.start.Y, v.end.Y) >= min(u.start.Y, u.end.Y)) &&
		(multiply(v.start, u.end, u.start)*multiply(u.end, v.end, u.start) >= 0) &&         //����ʵ�� 
		(multiply(u.start, v.end, v.start)*multiply(v.end, u.end, v.start) >= 0));
}

/* �жϵ�p�Ƿ����߶�l�ϣ�������(p���߶�l���ڵ�ֱ����)&& (��p�����߶�lΪ�Խ��ߵľ�����) */
bool online(line2d<double> l, vector2df p)
{
	return((multiply(l.end, p, l.start) == 0)
		&& (((p.X - l.start.X)*(p.X - l.end.X) <= 0) && ((p.Y - l.start.Y)*(p.Y - l.end.Y) <= 0)));
}


// ����߶�l1��l2�ཻ(ǰ���Ǳ����ཻ)������true�ҽ�����(inter)���أ����򷵻�false 
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




