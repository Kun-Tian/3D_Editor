#ifndef OFF2STL_H
#define OFF2STL_H

#include<string>

void getnorm(int va, int vb, int vc, float norm[]);
bool off2stl(const char * off_path, const char * stl_path);
float getheight(char *off_path);
struct point{
	float x, y, z;
	float norm[3];
	int NumOfFace;
	point(){
		NumOfFace = 0;
		norm[0] = 0;
		norm[1] = 0;
		norm[2] = 0;
	}
};

struct face{
	int a, b, c;
	float norm[3];
	face(){
		norm[0] = 0;
		norm[1] = 0;
		norm[2] = 0;
	}
};
#endif//OFF2STL_H