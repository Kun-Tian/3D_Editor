#ifndef _SOLVER
#define _SOLVER
extern "C" { 
#include "taucs.h" 
}
#include "Matrix.h"
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
using namespace std;

#pragma comment(lib, "libtaucs.lib")
#pragma comment(lib, "libf77blas.lib")
#pragma comment(lib, "libmetis.lib")
#pragma comment(lib, "vcf2c.lib")
#pragma comment(lib, "libatlas.lib")
#pragma comment(lib, "blas_win32.lib")
#pragma comment(lib, "lapack_win32.lib")
#pragma comment(lib, "libcblas.lib")
#pragma comment(lib, "liblapack.lib")

// 用 lu 分解来解 Ax = b，使用 memory_mb 的空间
int lu_solver(taucs_ccs_matrix *A, double *x, double *b, double memory_mb);


class LinearSolver // Ax=b
{
public:
	LinearSolver(int sz = 0);
	void resetBX();
	bool setMatrixToA(Matrix &adjMatrix);
	bool setVectorToB(vector<double> &vecb);
	// 得到taucs的数据结构，lower是说是否只要下半矩阵，用LLT必须为true，用LU必须为false
	taucs_ccs_matrix* matrix(bool lower);
	// 返回求解的 x 向量
	// positiveDefinite 是说是否是 对称正定矩阵，如果是，则用 LLT，否则用 LU
	// mmeory_mb 是说用 LU 时，使用多少内存
	double *solve(bool positiveDefinite = false, double memory_mb = 32);

public:
	int n; // A: nxn, x: nx1, b: nx1
	vector<double> x;
	vector<double> b;
	vector< map<int, double> > A;


};



#endif