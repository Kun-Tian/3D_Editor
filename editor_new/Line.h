#pragma once

#include "myMath.h"
#include "line2d.h"
#include <algorithm>
using namespace std;

class Line
{
public:
	//Line();
	Line(double d1=1, double d2=-1, double d3=0);
	~Line(void);
	Line makeline(vector2df p1, vector2df p2);
	bool lineintersect(Line l2, vector2df &p);

public:

	double a; 
	double b; 
	double c;
};


/****************************************************************************** 
r=multiply(sp,ep,op),得到(sp-op)*(ep-op)的叉积 
r>0:ep在矢量opsp的逆时针方向； 
r=0：opspep三点共线； 
r<0:ep在矢量opsp的顺时针方向 
******************************************************************************/ 

double multiply(vector2df sp, vector2df ep, vector2df op);




// 如果线段u和v相交(包括相交在端点处)时，返回true 
bool intersect(line2d<double> u, line2d<double> v);

/* 判断点p是否在线段l上，条件：(p在线段l所在的直线上)&& (点p在以线段l为对角线的矩形内) */ 
bool online(line2d<double> l, vector2df p);


// 如果线段l1和l2相交(前提是必须相交)，返回true且交点由(inter)返回，否则返回false 
bool intersection(line2d<double> l1, line2d<double> l2, vector2df &inter);

