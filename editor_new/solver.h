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

// �� lu �ֽ����� Ax = b��ʹ�� memory_mb �Ŀռ�
int lu_solver(taucs_ccs_matrix *A, double *x, double *b, double memory_mb);


class LinearSolver // Ax=b
{
public:
	LinearSolver(int sz = 0);
	void resetBX();
	bool setMatrixToA(Matrix &adjMatrix);
	bool setVectorToB(vector<double> &vecb);
	// �õ�taucs�����ݽṹ��lower��˵�Ƿ�ֻҪ�°������LLT����Ϊtrue����LU����Ϊfalse
	taucs_ccs_matrix* matrix(bool lower);
	// �������� x ����
	// positiveDefinite ��˵�Ƿ��� �Գ�������������ǣ����� LLT�������� LU
	// mmeory_mb ��˵�� LU ʱ��ʹ�ö����ڴ�
	double *solve(bool positiveDefinite = false, double memory_mb = 32);

public:
	int n; // A: nxn, x: nx1, b: nx1
	vector<double> x;
	vector<double> b;
	vector< map<int, double> > A;


};



#endif