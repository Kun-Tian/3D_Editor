#ifndef COMMON_STRUCTURE_H
#define COMMON_STRUCTURE_H


#include "vector3d.h"
#include "line3d.h"
#include "triangle3d.h"
//#include "meshOfHE.h"
#include <vector>

using namespace std;

//mesh3d


class mFacet //��������
{
public:
	int selfIndex;
	int indexOfPointA;
	int indexOfPointB;
	int indexOfPointC;
	int dirFromView; //����ӽǷ���,-1:δ��ʼ��,0:back,1:front
};
class mVert : public vector3df //����
{
public:
	int selfIndex;
};

class mObject //ʵ��
{
public:
	int selfIndex;
	int numOfVerts;
	int numOfFacets;

	vector<mVert> mVerts;
	vector<mFacet> mFacets;

//	HEMesh *meshHE;

	bool isHalfedge;
};

/*
//meshOfHE
class mVertHE : public mVert 
{
public:
	int indexOfOutHalfedge; //�Ӹõ������ĳһ������
};
class mHalfedgeHE
{
public:
	int selfIndex; //�������

	int indexOfNextHalfedge; //��̰��
	int indexOfOppoHalfedge; //��԰��
	int indexOfEndVert; //ָ��Ķ���

	int indexOfInEdge; //�����ı�
	int indexOfInFacet; //��������

	bool isBoundary; //�Ƿ�Ϊ�߽�
};
class mEdgeHE : public line3df
{
	int selfIndex; //�������
	
	int indexOfVertA;
	int indexOfVertB; //2����������
	int indexOfHalfedgeA;
	int indexOfHalfedgeB;

	bool isBoundary; //�Ƿ�Ϊ�߽�
	bool isOutline; //�Ƿ�Ϊ������
};
class mLoopHE //optional ��
{
	///////////to be edited
};
class mFacetHE : public mFacet
{
	int dir; //��ķ��� -1,0,1,����ͳһ�淽��ʱ������
	int indexOfOneHalfedge; //���ĳ�����
};
class mBoundaryHE //�߽�
{
public:
	int selfIndex; //���
	int lengthOfBoundary; //����
	int indexOfStartVert; //��ʼ��
	vector<int> halfedgeSeq; //����������
	bool isClose; //�Ƿ�ɻ�
};*/


#endif