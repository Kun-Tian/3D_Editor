#include "solver.h"


LinearSolver::LinearSolver(int sz)
{
	n = sz;
	x = vector<double>(n);
	b = vector<double>(n);
	A = vector< map<int, double> >(n);
}

void LinearSolver::resetBX()
{
	x.clear();
	x = vector<double>(n);
	b.clear();
	b = vector<double>(n);
}

bool LinearSolver::setMatrixToA(Matrix &adjMatrix)
{
	int k;
	if (adjMatrix.row != n || adjMatrix.col != n)
		return false;
	for (k = 0; k < adjMatrix.nElement; ++k)
	{
		if (adjMatrix.elements[k].v != 0)				//equal 0;
			A[adjMatrix.elements[k].i][adjMatrix.elements[k].j] = adjMatrix.elements[k].v;
	}
	return true;
}

bool LinearSolver::setVectorToB(vector<double> &vecb)
{
	int i;
	if (vecb.size() != n)
		return false;

	for (i = 0; i < n; ++i)
		b[i] = vecb[i];
	return true;
}

// 得到taucs的数据结构，lower是说是否只要下半矩阵，用LLT必须为true，用LU必须为false
taucs_ccs_matrix* LinearSolver::matrix(bool lower)
{
	//taucs中的对称矩阵
	int nzz = 0;
	int i, c, term;
	c = 0;
	term = 0;
	for (i = 0; i < n; i++)
	{
		term += A[i].size();
		//		cout<<"before: "<<AP[i].size()<<'\t';
		if (A[i][i] != 0)c++;
		//		cout<<"middle: "<<AP[i].size()<<'\t';
		if (A[i][i] == 0)A[i].erase(i);
		//		cout<<"after erase:"<<AP[i].size()<<'\t';
		//		cout<<endl;

	}
	//	if(lower==true){nzz=c+(term-c)/2;}
	//	else if(lower==false) nzz=term;
	nzz = (lower) ? (c + (term - c) / 2) : term;
	//cout<<nzz<<'\t'<<term<<'\t'<<c<<endl;
	taucs_ccs_matrix* mA = taucs_ccs_create(n, n, nzz, TAUCS_DOUBLE | (lower ? TAUCS_SYMMETRIC | TAUCS_LOWER : 0));

	mA->colptr[0] = nzz = 0;
	for (i = 0; i < n; i++)
	{
		for (map<int, double>::iterator it = A[i].begin(); it != A[i].end() && (!lower || it->first <= i); it++, nzz++)
		{
			mA->rowind[nzz] = it->first;
			mA->values.d[nzz] = it->second;
			//	cout<<i<<'\t'<<it->first<<'\t'<<it->second<<endl;
		}
		mA->colptr[i + 1] = nzz;
	}
	//cout<<'\n';
	/*	int index=0;
	for( i=0;i<n;i++){
	nzz=mA->colptr[i+1];
	cout<<"nZZ: "<<nzz<<endl;
	for(int j=index;j<nzz;j++){
	cout<<mA->values.d[index]<<'\t';
	index++;
	}
	}*/

	return mA;
}

// 返回求解的 x 向量
// positiveDefinite 是说是否是 对称正定矩阵，如果是，则用 LLT，否则用 LU
// mmeory_mb 是说用 LU 时，使用多少内存
double* LinearSolver::solve(bool positiveDefinite, double memory_mb)
{
	taucs_ccs_matrix *mA = matrix(positiveDefinite);
	string outStr;
	if (positiveDefinite)
	{
		char* options[] = { "taucs.factor.LLT=true", NULL };
		int ret = taucs_linsolve(mA, 0, 1, &x[0], &b[0], options, 0);

		switch (ret)
		{
		case TAUCS_SUCCESS:
			outStr = "success";
			break;
		case TAUCS_ERROR:
			outStr = "error";
			break;
		case TAUCS_ERROR_NOMEM:
			outStr = "error_nomen";
			break;
		case TAUCS_ERROR_BADARGS:
			outStr = "error_badargs";
			break;
		case TAUCS_ERROR_MAXDEPTH:
			outStr = "error_maxdepth";
			break;
		case TAUCS_ERROR_INDEFINITE:
			outStr = "error_indefinite";
			break;
		}
		cout << "linsolve ret: " << outStr << endl;
	}
	else
	{
		int ret = lu_solver(mA, &x[0], &b[0], memory_mb);
		switch (ret)
		{
		case TAUCS_SUCCESS:
			outStr = "success";
			break;
		case TAUCS_ERROR:
			outStr = "error";
			break;
		case TAUCS_ERROR_NOMEM:
			outStr = "error_nomen";
			break;
		case TAUCS_ERROR_BADARGS:
			outStr = "error_badargs";
			break;
		case TAUCS_ERROR_MAXDEPTH:
			outStr = "error_maxdepth";
			break;
		case TAUCS_ERROR_INDEFINITE:
			outStr = "error_indefinite";
			break;
		}
		cout << "linsolve ret: " << outStr << endl;
	}
	taucs_ccs_free(mA);
	return &x[0];
}

// 用 lu 分解来解 Ax = b，使用 memory_mb 的空间
int lu_solver(taucs_ccs_matrix *A, double *x, double *b, double memory_mb)
{
	int *perm = 0, *invperm = 0;
	taucs_ccs_order(A, &perm, &invperm, "colamd");
	taucs_io_handle* oocL = taucs_io_create_multifile("taucs.mf");
	int ret = (oocL == 0) || (perm == 0);
	if (ret == 0)
	{
		ret = taucs_ooc_factor_lu(A, perm, oocL, memory_mb * 1024 * 1024);
		if (ret == 0)
		{
			ret = taucs_ooc_solve_lu(oocL, x, b);
		}
	}
	taucs_io_delete(oocL);
	free(perm);
	free(invperm);
	return ret;
}
