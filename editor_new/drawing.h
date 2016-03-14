#pragma once

#include <windows.h>
#include <GL/gl.h>
#define GLUT_DISABLE_ATEXIT_HACK   
#include <GL/glut.h>

#include "commonstructure.h"
#include "mesh3d.h"
#include "myMath.h"
#include "triangle3d.h"
#include "vector2d.h"
#include "vector3d.h"
#include "Matrix.h"
#include "solver.h"
#include "line2d.h"
#include "Line.h"
#include "vector4d.h"
#include "lineqn.h"

#include <cmath>
#include <iostream>
#include <ctime>
#include <set>
#include <vector>
#include <string>
#include <queue>
using namespace std;


const int MaxWidth = 2000;
const int MaxHeight = 2000;
const GLfloat thetap = 0.0002;
const GLfloat thetan = 0.02;
const GLfloat Inf = 100000;
const int maxDfsDepth = 3;							//计算匹配分的递归深度
const int InfInt = 4000000;
const double InfDouble = 1e10;
const double boxl = 0.015;
const double depthTol = 0.01;

const double depthEp = 0.0001;

const int dir[8][2] =
{
	{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },				//4邻域
	{ 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 }				//8邻域
};


//int window2D, window3D;



struct QNode
{
	int i;
	double key;
};

bool operator< (const QNode &a, const QNode &b);			//大者优先


struct EdgeNode
{
	int i, j, k;
};


class Shape : public mesh3d
{
public:
	void draw();
	void init();
	void update();
	void outToFile(string filename);
	void edgeDetection();
	void interpolate(int i, int j, vector<double> v, vector3df &ans);
	void addEdgeSil(vector3df p1, vector3df p2, int vi1, int vj1, int vi2, int vj2);
	void addFaceSil(vector3df p1, vector3df p2, int vi1, int vj1, int f2);

public:
	vector<vector3df> triNorm;
	int nPoint;
	vector<vector3df> points;
	vector< set<int> > adj;							//双向边
	Matrix adjMatrix;
	vector<int> degree;
	vector< map<int, int> > iSilPoint;				//双向边
	vector<int> faceSilPoint;
	vector<vector3df> silPoints;
	vector<EdgeNode> silEdges;						//轮廓点在边上，返回边端点；轮廓点在面上，i表示面号
	vector<bool> isOnFace;
	vector<double> d;
	vector< set<int> > adjSilPoint;
	int nSilPoint;
	int curFace;

	//////////////////////////////////////////////////////////////////////////
public:
	struct BBox
	{
		vector3df min, max;
		vector3df center() const { return (min + max)*0.5f; }
		vector3df size() const { return max - min; }
		bool valid;
		BBox() : valid(false) {}
	};

	struct BSphere
	{
		vector3df center;
		float r;
		bool valid;
		BSphere() :valid(false) {}
	};

	void need_bbox();
	void need_bsphere();
	void need_curvatures();
	void need_normals();
	void need_pointareas();
	void need_dcurv();

	void need_neighbours();
	void need_adjacentfaces();
	void need_across_edge();

	void compute_viewdep_curv(int i, float ndotv, float u2, float uv, float v2, float &q1, vector2df &t1);
	void compute_Dt1q1(int i, float ndotv, const vector<float> &q1, const vector<vector2df> &t1, float &Dt1q1);
	void draw_segment_app_ridge(int v0, int v1, int v2,
		float emax0, float emax1, float emax2,
		float kmax0, float kmax1, float kmax2,
		const vector3df& tmax0, const vector3df& tmax1, const vector3df& tmax2,
		float thresh, bool to_center, bool do_test);
	void draw_face_app_ridges(int v0, int v1, int v2,
		const vector<float> &ndotv, const vector<float> &q1,
		const vector<vector2df> &t1, const vector<float>& Dt1q1,
		bool do_bfcull, bool do_test, float thresh);
	void draw_mesh_app_ridges(const vector<float> &ndotv, const vector<float>& q1,
		const vector<vector2df> &t1, const vector<float> &Dt1q1, bool do_bfcull, bool do_test, float thresh);
	void computer_feature_size();
	void setViewvec();
	BSphere bsphere;
protected:
	vector<vector3df> normals;
	vector<float> pointareas;
	vector<vector3df> cornerareas;
	vector<float> curv1, curv2;//principal curvature
	vector<vector3df> pdir1, pdir2;//principal curvature vector
	vector<vector4df> dcurv;// derivates of curvature
	vector< vector<int> > neighbour;
	vector< vector<int> > adjacentfaces;
	vector<mFacet> across_edge;


	double feature_size;
	BBox bbox;


	void compute_perview(vector<float> &ndotv, vector<float> &kr, vector<float> &sctest_num, vector<float> &sctest_den,
		vector<float> &shtest_num, vector<float> &q1, vector<vector2df> &t1, vector<float> &Dt1q1, bool extra_sin2theta = false);
	int farthest_vertex_along(const vector3df &dir);

	void draw_silhouette(const vector<float> &ndotv);
	vector3df gradkr(int i);
	float find_zero_linear(float val0, float val1) { return val0 / (val0 - val1); }; //Find a zero crossing between val0 and val1 by linear interpolation. Returns 0 if zero crossing is at val0 ,1 if at val1,etc
	float find_zero_hermite(int v0, int v1, float val0, float val1, const vector3df &grad0, const vector3df& grad1);
	void draw_face_isoline(int v0, int v1, int v2, const vector<float> &val, const vector<float> &test_num, const vector<float> &test_den, const vector<float> &ndotv, bool do_bfcull, bool do_hermite, bool do_test, float fade);
	void draw_face_isoline2(int v0, int v1, int v2, const vector<float> &val, const vector<float> &test_num, const vector<float> &test_den, bool do_hermite, bool do_test, float fade);
	void draw_isolines(const vector<float> &val, const vector<float> &test_num, const vector<float> &test_den, const vector<float> &ndotv, bool do_bfcull, bool do_hermite, bool do_test, float fade);
	void draw_misc(const vector<float> &ndotv, const vector<float> &DwKr, bool do_hidden);
	void draw_mesh_ridges(bool do_ridge, const vector<float> &ndotv, bool do_bfcull, bool do_test, float thresh);
	void draw_face_ridges(int v0, int v1, int v2, bool do_ridge, const vector<float> &ndotv, bool do_bfcull, bool do_test, float thresh);
	void draw_segment_ridge(int v0, int v1, int v2, float emax0, float emax1, float emax2, float kmax0, float kmax1, float kmax2, float thresh, bool to_center);
	void draw_mesh_ph(bool do_ridge, const vector<float> &ndotv, bool do_bfcull, bool do_test, float thresh);
	void draw_face_ph(int v0, int v1, int v2, bool do_ridge, const vector<float> &ndotv, bool do_bfcull, bool do_test, float thresh);
	void draw_boundaries(bool do_hidden);
	void draw_isophotes(const vector<float> &ndotv);
	void draw_topolines(const vector<float> &ndotv);
	//////////////////////////////////////////////////////////////////////////

};



class Image
{
public:
	void init();
	void drawShape();
	void bigger();
	void smaller();
	void turnLeft();
	void turnRight();
	void turnUp();
	void turnDown();
	void turnS();
	void turnN();
	void getViewVector();
	void matchRefStroke();																			//依赖于refStroke，得到matchSil3D
	void reconStroke();																				//依赖于handleSilPointProj2D和defStroke，得到reDefStroke3D
	void findHandlePoint();																			//依赖于handleSilPoint和reDefStroke3D，得到handlePoint和corrPos
	void findAncher();																				//依赖于handlePoint和corrPos，添加handlePoint和corrPos
	void changePos();																				//变形
	void drawSil();
	void drawStroke(vector<vector3df> &stroke);
	void resetStroke();
	void checkEdges();
	void checkVisable();
	double checkDepth(vector3df win);


	void getPoint3D(int i, int j, double r, vector3df &ans);
	void transToObjCood(double x, double y, double z, double &tx, double &ty, double &tz);				//将窗口坐标转换为三维空间坐标
	void transToWinCood(double x, double y, double z, double &tx, double &ty, double &tz);			//将三维空间坐标转换为窗口坐标	
	bool borderCan(int i, int j);
	void tracing();
	void dfsTrace(vector<int> &line, int i);
	int matchScore(int iSil, int silStart, int silEnd, int refStart, int refEnd, int dfsDepth);
	double distance(vector3df p1, vector3df p2);
	double distance(vector2di p1, vector2di p2);
	double distance(vector2df p1, vector2df p2);
	double distance2(vector2df p1, vector2df p2);
	int distance2(vector2di v1, vector2di v2);
	double distance2(vector3df p1, vector3df p2);
	vector3df insert(vector3df s, vector3df e, double len);
	void transToWinCood3D(vector3df p, vector3df &win);

	~Image();

public:
	Shape myShape;
	//窗口宽高
	GLsizei width, height;
	GLsizei width3D, height3D;
	//缩放，旋转
	GLfloat scale, scale3D, angley3D, anglex3D, anglez3D;
	//当前矩阵
	GLdouble modelMatrix[16], projMatrix[16];
	GLint viewport[4];
	GLdouble modelMatrix3D[16], projMatrix3D[16];
	GLint viewport3D[4];

	bool colorSil;

	//边缘检测
	float depthMap[MaxHeight*MaxWidth];
	//手绘笔画
	vector<vector2df> defStroke, matchSil;
	vector<vector3df> defStroke3D, matchSil3D, reDefStroke3D;
	vector<int> matchSilIndex;
	//handle点和ROI
	vector<int> handlePoint;
	int *isHandle;
	int nHandle;
	//关联点
	vector<vector3df> corrPos;
	double rate;
	Matrix solveMatrix, mMatrix, mtMatrix, MTM;

	vector<vector2df> silPoint2D;
	vector< vector<int> > sil;
	vector<bool> cycle;
	int nSil;
	vector<bool> traced, visable;
	bool ready;
	vector<double> silPointDepth;

};
