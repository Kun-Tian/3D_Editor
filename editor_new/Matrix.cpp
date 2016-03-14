#include "Matrix.h"

bool cmp(Tuple a, Tuple b)
{
	return a.i < b.i;
}

void Matrix::reset()
{
	nElement = 0;
	elements.clear();
	rowPos.clear();
}

void Matrix::insert(int i, int j, double v)
{
	Tuple a;
	a.i = i; a.j = j; a.v = v;
	elements.push_back(a);
	nElement++;
}

void Matrix::setStructure(int r, int c)
{
	row = r; col = c;
	rowPos = vector<int>(row);
	adjust();
}

void Matrix::adjust()
{
	int k;
	sort(elements.begin(), elements.end(), cmp);
	for (k = 0; k < row; ++k)
		rowPos[k] = -1;
	for (k = 0; k < nElement; ++k)
		if (rowPos[elements[k].i] == -1)
			rowPos[elements[k].i] = k;
}

void Matrix::build(vector< map<int, double> > mp, int r, int c)
{
	int i;
	map<int, double>::iterator it;
	reset();
	for (i = 0; i < r; ++i)
		for (it = mp[i].begin(); it != mp[i].end(); it++)
			insert(i, it->first, it->second);
	setStructure(r, c);
	adjust();
}

Matrix& Matrix::operator=(const Matrix& other)
{
	int k;
	Tuple cur;
	reset();
	setStructure(other.row, other.col);
	nElement = other.nElement;
	for (k = 0; k < nElement; ++k)
	{
		cur.i = other.elements[k].i;
		cur.j = other.elements[k].j;
		cur.v = other.elements[k].v;
		elements.push_back(cur);
	}
	for (k = 0; k < row; ++k)
		rowPos[k] = other.rowPos[k];
	return *this;
}

void Matrix::trans(Matrix &b)
{
	int k;
	Tuple cur;

	b.reset();
	b.setStructure(col, row);
	for (k = 0; k < nElement; ++k)
	{
		cur.i = elements[k].j;
		cur.j = elements[k].i;
		cur.v = elements[k].v;
		b.elements.push_back(cur);
		++b.nElement;
	}
	b.adjust();
}

bool Matrix::multi(Matrix b, Matrix &c)
{
	int k, l;
	vector< map<int, double> > tmp;

	if (col != b.row)
		return false;

	tmp = vector< map<int, double> >(row);
	for (k = 0; k < nElement; ++k)
	{
		if (b.rowPos[elements[k].j] != -1)
			for (l = b.rowPos[elements[k].j]; l < b.nElement && b.elements[l].i == elements[k].j; ++l)
				tmp[elements[k].i][b.elements[l].j] += elements[k].v * b.elements[l].v;
	}
	c.build(tmp, row, b.col);
	return true;
}

void Matrix::transMulti(Matrix &c)
{
	int i, k, l;
	vector< map<int, double> > tmp;

	tmp = vector< map<int, double> >(col);
	for (i = 0; i < row; ++i)
		if (rowPos[i] != -1)
			for (k = rowPos[i]; k < nElement && elements[k].i == i; ++k)
				for (l = rowPos[i]; l < nElement && elements[l].i == i; ++l)
					tmp[elements[k].j][elements[l].j] += elements[k].v * elements[l].v;
	c.build(tmp, col, col);
}

void Matrix::getRows(int n, Matrix &res)
{
	int k;
	res.reset();
	for (k = 0; k < nElement; ++k)
		if (elements[k].i < n)
			res.insert(elements[k].i, elements[k].j, elements[k].v);
	res.setStructure(n, col);
	res.adjust();
}

bool Matrix::changeToVec(vector<double> &vecb)
{
	int k;
	if (col > 1)
		return false;
	vecb = vector<double>(row);
	for (k = 0; k < nElement; ++k)
		vecb[elements[k].i] = elements[k].v;
	return true;
}
