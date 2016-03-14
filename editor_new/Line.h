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
r=multiply(sp,ep,op),�õ�(sp-op)*(ep-op)�Ĳ�� 
r>0:ep��ʸ��opsp����ʱ�뷽�� 
r=0��opspep���㹲�ߣ� 
r<0:ep��ʸ��opsp��˳ʱ�뷽�� 
******************************************************************************/ 

double multiply(vector2df sp, vector2df ep, vector2df op);




// ����߶�u��v�ཻ(�����ཻ�ڶ˵㴦)ʱ������true 
bool intersect(line2d<double> u, line2d<double> v);

/* �жϵ�p�Ƿ����߶�l�ϣ�������(p���߶�l���ڵ�ֱ����)&& (��p�����߶�lΪ�Խ��ߵľ�����) */ 
bool online(line2d<double> l, vector2df p);


// ����߶�l1��l2�ཻ(ǰ���Ǳ����ཻ)������true�ҽ�����(inter)���أ����򷵻�false 
bool intersection(line2d<double> l1, line2d<double> l2, vector2df &inter);

