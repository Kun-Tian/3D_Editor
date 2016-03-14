#ifndef COMMON_STRUCTURE_H
#define COMMON_STRUCTURE_H


#include "vector3d.h"
#include "line3d.h"
#include "triangle3d.h"
//#include "meshOfHE.h"
#include <vector>

using namespace std;

//mesh3d


class mFacet //三角形面
{
public:
	int selfIndex;
	int indexOfPointA;
	int indexOfPointB;
	int indexOfPointC;
	int dirFromView; //面的视角方向,-1:未初始化,0:back,1:front
};
class mVert : public vector3df //顶点
{
public:
	int selfIndex;
};

class mObject //实体
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
	int indexOfOutHalfedge; //从该点出发的某一条出边
};
class mHalfedgeHE
{
public:
	int selfIndex; //自身序号

	int indexOfNextHalfedge; //后继半边
	int indexOfOppoHalfedge; //相对半边
	int indexOfEndVert; //指向的顶点

	int indexOfInEdge; //所属的边
	int indexOfInFacet; //所属的面

	bool isBoundary; //是否为边界
};
class mEdgeHE : public line3df
{
	int selfIndex; //自身序号
	
	int indexOfVertA;
	int indexOfVertB; //2个顶点的序号
	int indexOfHalfedgeA;
	int indexOfHalfedgeB;

	bool isBoundary; //是否为边界
	bool isOutline; //是否为轮廓线
};
class mLoopHE //optional 环
{
	///////////to be edited
};
class mFacetHE : public mFacet
{
	int dir; //面的方向 -1,0,1,用于统一面方向时候辅助用
	int indexOfOneHalfedge; //面的某条半边
};
class mBoundaryHE //边界
{
public:
	int selfIndex; //序号
	int lengthOfBoundary; //长度
	int indexOfStartVert; //起始点
	vector<int> halfedgeSeq; //半边序号序列
	bool isClose; //是否成环
};*/


#endif