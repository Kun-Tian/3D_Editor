#define _CRT_SECURE_NO_WARNINGS
#include"off2stl.h"
#include<iostream>
using namespace std;

struct point *vh;
struct face *fh;
int num_of_v;
int num_of_f;


bool off2stl(const char * off_path,const char * stl_path)
{
	FILE *off_fp = fopen(off_path, "r");//读off文件
	if (off_fp == NULL)
	{
		return false;
	}
	char off[10];
	int NumOfV, NumOfF, NumOfE;
	int currentedge = 0, nump, currentpart = 0;
	fscanf(off_fp, "%s", off);
	if (strcmp(off, "OFF") != 0)
	{
		return false;
	}
	fscanf(off_fp, "%d", &NumOfV);
	fscanf(off_fp, "%d", &NumOfF);
	fscanf(off_fp, "%d", &NumOfE);
	num_of_v = NumOfV;
	num_of_f = NumOfF;
	cout << num_of_v;
	vh = new point[NumOfV];
	fh = new face[NumOfF];
	/*获得整个模型的点集*/
	for (int i = 0; i<NumOfV; i++)
	{
		fscanf(off_fp, "%f", &vh[i].x);
		fscanf(off_fp, "%f", &vh[i].y);
		fscanf(off_fp, "%f", &vh[i].z);
	}
	/*获得整个模型的面集*/
	for (int i = 0; i<NumOfF; i++)
	{
		fscanf(off_fp, "%d", &nump);
		fscanf(off_fp, "%d", &fh[i].a);
		fscanf(off_fp, "%d", &fh[i].b);
		fscanf(off_fp, "%d", &fh[i].c);

		/* 求三角面法向量 */
		int va = fh[i].a, vb = fh[i].b, vc = fh[i].c;
		getnorm(va, vb, vc, fh[i].norm);


	}
	fclose(off_fp);

	float x_min = vh[0].x, y_min = vh[0].y, z_min = vh[0].z;//计算坐标偏移量
	for (int i = 0; i < NumOfV; i++)
	{
		if (vh[i].x < x_min)x_min = vh[i].x;
		if (vh[i].y < y_min)y_min = vh[i].y;
		if (vh[i].z < z_min)z_min = vh[i].z;
	}
	
	x_min = 0;
	y_min = 0;
	z_min = 0;

	cout << x_min << " " << y_min << " " << z_min<<endl;


	FILE *stl_fp = fopen(stl_path, "w");//写stl文件
	fprintf(stl_fp, "solid\n");
	for (int i = 0; i < NumOfF; i++)
	{
		fprintf(stl_fp, "  facet normal %e %e %e\n", fh[i].norm[0], fh[i].norm[1], fh[i].norm[2]);
		fprintf(stl_fp, "    outer loop\n");
		fprintf(stl_fp, "      vertex  %e %e %e\n", vh[fh[i].a].x - x_min, vh[fh[i].a].y - y_min, vh[fh[i].a].z - z_min);
		fprintf(stl_fp, "      vertex  %e %e %e\n", vh[fh[i].b].x - x_min, vh[fh[i].b].y - y_min, vh[fh[i].b].z - z_min);
		fprintf(stl_fp, "      vertex  %e %e %e\n", vh[fh[i].c].x - x_min, vh[fh[i].c].y - y_min, vh[fh[i].c].z - z_min);
		fprintf(stl_fp, "    endloop\n");
		fprintf(stl_fp, "  endfacet\n");
	}
	fprintf(stl_fp, "endsolid");
	fclose(stl_fp);

	return true;
}

void getnorm(int va, int vb, int vc, float norm[])//p q r 
{
	float vc1[3], vc2[3];
	float t1, t2, t3, r;
	vc1[0] = vh[vb].x - vh[va].x; vc1[1] = vh[vb].y - vh[va].y; vc1[2] = vh[vb].z - vh[va].z;//q-p
	vc2[0] = vh[vc].x - vh[va].x; vc2[1] = vh[vc].y - vh[va].y; vc2[2] = vh[vc].z - vh[va].z;//r-p
	t1 = vc1[1] * vc2[2] - vc2[1] * vc1[2];//
	t2 = vc2[0] * vc1[2] - vc1[0] * vc2[2];//(q-p)X(r-p)
	t3 = vc1[0] * vc2[1] - vc2[0] * vc1[1];//
	r = sqrt(t1 * t1 + t2 * t2 + t3 * t3);
	if (r != 0){
		norm[0] = t1 / r; norm[1] = t2 / r; norm[2] = t3 / r;
	}
	else{
		norm[0] = 0; norm[1] = 0; norm[2] = 0;
	}
}