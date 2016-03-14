#ifndef MATRIX_H
#define MATRIX_H
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>
using namespace std;

const int MaxRow = 1000;
const double ep = 1e-10;

struct Tuple
{
	int i, j;
	double v;
};

bool cmp(Tuple a, Tuple b);

class Matrix
{
public:
	void reset();
	void insert(int i, int j, double v);
	void setStructure(int r, int c);
	void adjust();
	void build(vector< map<int, double> > mp, int r, int c);
	Matrix& operator=(const Matrix& other);
	void trans(Matrix &b);					//b = 当前矩阵的转置
	bool multi(Matrix b, Matrix &c);		//c = 当前矩阵 * b	
	void transMulti(Matrix &c);				//c = aT*a
	void getRows(int n, Matrix &res);
	bool changeToVec(vector<double> &vecb);	

public:
	int row, col;
	int nElement;
	vector<Tuple> elements;
	vector<int> rowPos;
};


#endif