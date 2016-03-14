#include "drawing.h"

#ifdef __cplusplus
extern "C"
#endif
FILE _iob[3] = { __iob_func()[0], __iob_func()[1], __iob_func()[2] };
int __mb_cur_max;
const unsigned short* _pctype;

string inFileName = "data/2.off";
string outFileName;

//////////////////////////////////////////////////////////////////////////
double fov = 0.7;
bool draw_mesh = false;
bool draw_faded = 0;
bool draw_sc = true;									//
bool draw_sh = false;									//
bool draw_DwKr = false;
bool draw_apparent = false;							//
bool draw_extsil = true;								//
bool draw_hidden = false;
bool draw_colors = false;
bool draw_K = false;
bool draw_H = false;
bool draw_edges = false;
bool draw_ridges = true;								//
bool draw_valleys = false;							//
bool draw_phridges = false, draw_phvalleys = false;		//
bool draw_bdy = false;
bool draw_c = true;									//
bool draw_isoph = false;
bool draw_topo = false;
bool test_ar = true, test_c = true, test_sc = true, test_rv = true, test_ph = true, test_sh = true;
bool use_hermite = false;
float rv_thresh = 0.1, ar_thresh = 0.1;
float sug_thresh = 0.01, sh_thresh = 0.02, ph_thresh = 0.04;
int mx = 0, my = 0;
vector3df viewVec;
//////////////////////////////////////////////////////////////////////////

Image myImage;

bool operator< (const QNode &a, const QNode &b)				//大者优先
{
	return a.key < b.key;
}

void Shape::init()
{
	int i;
	set<int> line;
	set<int>::iterator it;

	loadFromOFF(inFileName);

	points.clear();
	nPoint = mObjects[0].numOfVerts;
	for (i = 0; i < nPoint; ++i)
		points.push_back(mObjects[0].mVerts[i]);

	adj.clear();
	line.clear();
	for (i = 0; i < nPoint; ++i)
		adj.push_back(line);

	for (i = 0; i < mObjects[0].numOfFacets; ++i)
	{
		adj[mObjects[0].mFacets[i].indexOfPointA].insert(mObjects[0].mFacets[i].indexOfPointB);
		adj[mObjects[0].mFacets[i].indexOfPointB].insert(mObjects[0].mFacets[i].indexOfPointA);
		adj[mObjects[0].mFacets[i].indexOfPointA].insert(mObjects[0].mFacets[i].indexOfPointC);
		adj[mObjects[0].mFacets[i].indexOfPointC].insert(mObjects[0].mFacets[i].indexOfPointA);
		adj[mObjects[0].mFacets[i].indexOfPointC].insert(mObjects[0].mFacets[i].indexOfPointB);
		adj[mObjects[0].mFacets[i].indexOfPointB].insert(mObjects[0].mFacets[i].indexOfPointC);
	}

	degree = vector<int>(nPoint);
	adjMatrix.reset();
	for (i = 0; i < nPoint; ++i)
	{
		degree[i] = adj[i].size();
		adjMatrix.insert(i, i, 1);
		for (it = adj[i].begin(); it != adj[i].end(); it++)
			adjMatrix.insert(i, *it, -1.0 / degree[i]);
	}
	adjMatrix.setStructure(nPoint, nPoint);
	d = vector<double>(nPoint);
	nSilPoint = 0;

}

void Shape::update()
{
	int i, j;
	triangle3df curTri;
	vector3df curVec;

	triNorm.clear();
	for (i = 0; i < numOfObjects; ++i)
	{
		for (j = 0; j < mObjects[i].numOfFacets; ++j)
		{
			curTri.set(points[mObjects[i].mFacets[j].indexOfPointA],
				points[mObjects[i].mFacets[j].indexOfPointB],
				points[mObjects[i].mFacets[j].indexOfPointC]);
			curVec = curTri.getNormal();
			triNorm.push_back(curVec);
		}
	}


	for (i = 0; i < nPoint; ++i)
	{
		mObjects[0].mVerts[i].X = points[i].X;
		mObjects[0].mVerts[i].Y = points[i].Y;
		mObjects[0].mVerts[i].Z = points[i].Z;
	}


	need_bsphere();
	need_normals();
	need_curvatures();
	need_dcurv();
	computer_feature_size();

}

void Shape::outToFile(string filename)
{
	ofstream outFile(filename.c_str());
	int i;

	if (!outFile)
	{
		cerr << "File '" << filename.c_str() << "' failed to load!" << endl;
		exit(0);
	}

	outFile << "OFF\n";
	outFile << mObjects[0].numOfVerts << " " << mObjects[0].numOfFacets << " 0\n";

	for (i = 0; i < mObjects[0].numOfVerts; i++)
	{
		outFile << mObjects[0].mVerts[i].X << " " << mObjects[0].mVerts[i].Y << " " << mObjects[0].mVerts[i].Z << endl;
	}
	for (i = 0; i < mObjects[0].numOfFacets; i++)
	{
		outFile << "3 " << mObjects[0].mFacets[i].indexOfPointA << " " << mObjects[0].mFacets[i].indexOfPointB << " " << mObjects[0].mFacets[i].indexOfPointC << endl;
	}

	outFile.clear();
}

void Shape::addEdgeSil(vector3df p1, vector3df p2, int vi1, int vj1, int vi2, int vj2)
{
	int ip1, ip2;
	EdgeNode curEdge;

	ip1 = iSilPoint[vi1][vj1];
	if (ip1 == -1)
	{
		silPoints.push_back(p1);
		curEdge.i = vi1; curEdge.j = vj1;
		silEdges.push_back(curEdge);
		iSilPoint[vi1][vj1] = iSilPoint[vj1][vi1] = nSilPoint;
		ip1 = nSilPoint;
		isOnFace.push_back(false);
		++nSilPoint;
	}

	ip2 = iSilPoint[vi2][vj2];
	if (ip2 == -1)
	{
		silPoints.push_back(p2);
		curEdge.i = vi2; curEdge.j = vj2;
		silEdges.push_back(curEdge);
		iSilPoint[vi2][vj2] = iSilPoint[vj2][vi2] = nSilPoint;
		ip2 = nSilPoint;
		isOnFace.push_back(false);
		++nSilPoint;
	}

	adjSilPoint[ip1].insert(ip2);
	adjSilPoint[ip2].insert(ip1);

}

void Shape::addFaceSil(vector3df p1, vector3df p2, int vi1, int vj1, int f2)
{
	int ip;
	EdgeNode curEdge;

	ip = iSilPoint[vi1][vj1];
	if (ip == -1)
	{
		silPoints.push_back(p1);
		curEdge.i = vi1; curEdge.j = vj1;
		silEdges.push_back(curEdge);
		iSilPoint[vi1][vj1] = iSilPoint[vj1][vi1] = nSilPoint;
		ip = nSilPoint;
		isOnFace.push_back(false);
		++nSilPoint;
	}

	if (faceSilPoint[f2] == -1)
	{
		silPoints.push_back(p2);
		curEdge.i = mObjects[0].mFacets[f2].indexOfPointA;
		curEdge.j = mObjects[0].mFacets[f2].indexOfPointB;
		curEdge.k = mObjects[0].mFacets[f2].indexOfPointC;
		silEdges.push_back(curEdge);
		faceSilPoint[f2] = nSilPoint;
		isOnFace.push_back(true);
		++nSilPoint;
	}


	adjSilPoint[ip].insert(faceSilPoint[f2]);
	adjSilPoint[faceSilPoint[f2]].insert(ip);

}

void Shape::draw()
{
	if (points.empty())
		return;

	int i, j;


	for (i = 0; i < numOfObjects; ++i)
	{
		glBegin(GL_TRIANGLES);
		for (j = 0; j < mObjects[i].numOfFacets; ++j)
		{
			triNorm[j].normalize();
			glNormal3f(triNorm[j].X, triNorm[j].Y, triNorm[j].Z);
			glVertex3f(points[mObjects[i].mFacets[j].indexOfPointA].X,
				points[mObjects[i].mFacets[j].indexOfPointA].Y,
				points[mObjects[i].mFacets[j].indexOfPointA].Z);

			glVertex3f(points[mObjects[i].mFacets[j].indexOfPointB].X,
				points[mObjects[i].mFacets[j].indexOfPointB].Y,
				points[mObjects[i].mFacets[j].indexOfPointB].Z);

			glVertex3f(points[mObjects[i].mFacets[j].indexOfPointC].X,
				points[mObjects[i].mFacets[j].indexOfPointC].Y,
				points[mObjects[i].mFacets[j].indexOfPointC].Z);

		}
		glEnd();
	}

}


void Shape::edgeDetection()
{
	int i;
	vector3df vecab, vecac, vecbc, vec2D;
	map<int, int>::iterator it, it1;
	//	EdgeNode curEdge;
	//	double tmp;


	for (i = 0; i < nPoint; ++i)
	{
		d[i] = viewVec.dotProduct(normals[i]) / normals[i].getLength() / viewVec.getLength();
	}


	iSilPoint.clear();
	iSilPoint = vector< map<int, int> >(nPoint);
	faceSilPoint.clear();
	faceSilPoint = vector<int>(mObjects[0].numOfFacets);
	for (i = 0; i < mObjects[0].numOfFacets; ++i)
	{
		faceSilPoint[i] = -1;
		iSilPoint[mObjects[0].mFacets[i].indexOfPointA][mObjects[0].mFacets[i].indexOfPointB] = -1;
		iSilPoint[mObjects[0].mFacets[i].indexOfPointB][mObjects[0].mFacets[i].indexOfPointA] = -1;
		iSilPoint[mObjects[0].mFacets[i].indexOfPointA][mObjects[0].mFacets[i].indexOfPointC] = -1;
		iSilPoint[mObjects[0].mFacets[i].indexOfPointC][mObjects[0].mFacets[i].indexOfPointA] = -1;
		iSilPoint[mObjects[0].mFacets[i].indexOfPointB][mObjects[0].mFacets[i].indexOfPointC] = -1;
		iSilPoint[mObjects[0].mFacets[i].indexOfPointC][mObjects[0].mFacets[i].indexOfPointB] = -1;
	}

	nSilPoint = 0;
	silPoints.clear();
	silEdges.clear();
	isOnFace.clear();
	adjSilPoint.clear();
	adjSilPoint = vector< set<int> >(mObjects[0].numOfFacets * 4);

	for (i = 0; i < mObjects[0].numOfFacets; ++i)
	{
		if (d[mObjects[0].mFacets[i].indexOfPointA] * d[mObjects[0].mFacets[i].indexOfPointB] <= 0)			//ab
		{
			interpolate(mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointB, d, vecab);
			if (d[mObjects[0].mFacets[i].indexOfPointA] * d[mObjects[0].mFacets[i].indexOfPointC] <= 0)		//ac
			{
				interpolate(mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointC, d, vecac);
				addEdgeSil(vecab, vecac, mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointC);
			}
			if (d[mObjects[0].mFacets[i].indexOfPointB] * d[mObjects[0].mFacets[i].indexOfPointC] <= 0)		//bc
			{
				interpolate(mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointC, d, vecbc);
				addEdgeSil(vecab, vecbc, mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointC);
			}
		}
		else
		{
			if (d[mObjects[0].mFacets[i].indexOfPointA] * d[mObjects[0].mFacets[i].indexOfPointC] <= 0 &&	//ac
				d[mObjects[0].mFacets[i].indexOfPointB] * d[mObjects[0].mFacets[i].indexOfPointC] <= 0)	//bc	
			{

				interpolate(mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointC, d, vecac);
				interpolate(mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointC, d, vecbc);
				addEdgeSil(vecac, vecbc, mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointC, mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointC);
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////

	//These are static so the memory isn`t reallocated on every frame
	static vector<float> ndotv, kr;
	static vector<float> sctest_num, sctest_den, shtest_num;
	static vector<float> q1, Dt1q1;
	static vector<vector2df> t1;
	//setViewvec();
	compute_perview(ndotv, kr, sctest_num, sctest_den, shtest_num, q1, t1, Dt1q1);
	int nv = mObjects[0].mVerts.size();


	//Exterior silhouette
	if (draw_extsil)
		draw_silhouette(ndotv);

	{

		draw_misc(ndotv, sctest_num, true);

		//Apparent ridges
		if (draw_apparent)
		{
			draw_mesh_app_ridges(ndotv, q1, t1, Dt1q1, true, test_ar, ar_thresh / (feature_size*feature_size));
		}

		//Ridges and valleys
		if (draw_ridges)
		{
			draw_mesh_ridges(true, ndotv, false, test_rv, rv_thresh / feature_size);
		}
		if (draw_valleys)
		{
			draw_mesh_ridges(false, ndotv, false, test_rv, rv_thresh / feature_size);
		}

		//Principal highlights
		if (draw_phridges || draw_phvalleys)
		{
			float thresh = ph_thresh / (feature_size*feature_size);
			if (draw_phridges)
				draw_mesh_ph(true, ndotv, false, test_ph, thresh);
			if (draw_phvalleys)
				draw_mesh_ph(false, ndotv, false, test_ph, thresh);
		}

		//Suggestive highlights
		if (draw_sh)
		{
			float fade = draw_faded ? 0.03f / (feature_size*feature_size) : 0.0f;
			draw_isolines(kr, shtest_num, sctest_den, ndotv, false, use_hermite, test_sh, fade);
		}

		//Suggestive contours and contours
		if (draw_sc)
		{
			float fade = (draw_faded&&test_sc) ? 0.03f / (feature_size*feature_size) : 0.0f;
			draw_isolines(kr, sctest_num, sctest_den, ndotv, false, use_hermite, test_sc, fade);
		}

	}


	//////////////////////////////////////////////////////////////////////////



}


void Shape::interpolate(int i, int j, vector<double> v, vector3df &ans)
{
	ans = points[i] * fabs(v[j]) / (fabs(v[i]) + fabs(v[j])) +
		points[j] * fabs(v[i]) / (fabs(v[i]) + fabs(v[j]));
}


//////////////////////////////////////////////////////////////////////////
// Rotate a coordinate system to be perpendicular to the given normal
void rot_coord_sys(const vector3df &old_u, const vector3df &old_v,
	const vector3df &new_norm,
	vector3df &new_u, vector3df &new_v)
{
	new_u = old_u;
	new_v = old_v;
	vector3df old_norm = old_u.crossProduct(old_v);
	float ndot = old_norm.dotProduct(new_norm);
	if (unlikely(ndot <= -1.0f))
	{
		new_u = -new_u;
		new_v = -new_v;
		return;
	}
	vector3df perp_old = new_norm - old_norm*ndot;
	vector3df dperp = (old_norm + new_norm) / (1 + ndot);
	new_u -= dperp*(new_u.dotProduct(perp_old));
	new_v -= dperp*(new_v.dotProduct(perp_old));
}


// Reproject a curvature tensor from the basis spanned by old_u and old_v
// (which are assumed to be unit-length and perpendicular) to the
// new_u, new_v basis.
void proj_curv(const vector3df &old_u, const vector3df &old_v,
	float old_ku, float old_kuv, float old_kv,
	const vector3df &new_u, const vector3df &new_v,
	float &new_ku, float &new_kuv, float &new_kv)
{
	vector3df r_new_u, r_new_v;
	rot_coord_sys(new_u, new_v, old_u.crossProduct(old_v), r_new_u, r_new_v);

	float u1 = r_new_u.dotProduct(old_u);
	float v1 = r_new_u.dotProduct(old_v);
	float u2 = r_new_v.dotProduct(old_u);
	float v2 = r_new_v.dotProduct(old_v);
	new_ku = old_ku*u1*u1 + old_kuv*(2.0f*u1*v1) + old_kv*v1*v1;
	new_kuv = old_ku*u1*u2 + old_kuv*(u1*v2 + u2*v1) + old_kv*v1*v2;
	new_kv = old_ku*u2*u2 + old_kuv*(2.0f*u2*v2) + old_kv*v2*v2;
}


// Like the above, but for dcurv
void proj_dcurv(const vector3df &old_u, const vector3df &old_v,
	const vector4df old_dcurv,
	const vector3df &new_u, const vector3df &new_v,
	vector4df &new_dcurv)
{
	vector3df r_new_u, r_new_v;
	rot_coord_sys(new_u, new_v, old_u.crossProduct(old_v), r_new_u, r_new_v);

	float u1 = r_new_u.dotProduct(old_u);
	float v1 = r_new_u.dotProduct(old_v);
	float u2 = r_new_v.dotProduct(old_u);
	float v2 = r_new_v.dotProduct(old_v);

	new_dcurv.X = old_dcurv.X*u1*u1*u1 + old_dcurv.Y*3.0f*u1*u1*v1 + old_dcurv.Z*3.0f*u1*v1*v1 + old_dcurv.W*v1*v1*v1;
	new_dcurv.Y = old_dcurv.X*u1*u1*u2 + old_dcurv.Y*(u1*u1*v2 + 2.0f*u2*u1*v1) + old_dcurv.Z*(u2*v1*v1 + 2.0f*u1*v1*v2) + old_dcurv.W*v1*v1*v2;
	new_dcurv.Z = old_dcurv.X*u1*u2*u2 + old_dcurv.Y*(u2*u2*v1 + 2.0f*u1*u2*v2) + old_dcurv.Z*(u1*v2*v2 + 2.0f*u2*v2*v1) + old_dcurv.W*v1*v2*v2;
	new_dcurv.W = old_dcurv.X*u2*u2*u2 + old_dcurv.Y*3.0f*u2*u2*v2 + old_dcurv.Z*3.0f*u2*v2*v2 + old_dcurv.W*v2*v2*v2;
}


// Given a curvature tensor, find principal directions and curvatures
// Makes sure that pdir1 and pdir2 are perpendicular to normal
void diagonalize_curv(const vector3df &old_u, const vector3df &old_v,
	float ku, float kuv, float kv,
	const vector3df &new_norm,
	vector3df &pdir1, vector3df &pdir2, float &k1, float &k2)
{
	vector3df r_old_u, r_old_v;
	rot_coord_sys(old_u, old_v, new_norm, r_old_u, r_old_v);

	float c = 1, s = 0, tt = 0;
	if (likely(kuv != 0.0f))
	{
		// Jacobi rotation to diagonalize
		float h = 0.5f*(kv - ku) / kuv;
		tt = (h<0.0f) ? 1.0f / (h - sqrt(1.0f + h*h)) : 1.0f / (h + sqrt(1.0f + h*h));
		c = 1.0f / sqrt(1.0f + tt*tt);
		s = tt*c;
	}

	k1 = ku - tt*kuv;
	k2 = kv + tt*kuv;

	if (fabs(k1) >= fabs(k2))
	{
		pdir1 = r_old_u*c - r_old_v*s;
	}
	else
	{
		swap(k1, k2);
		pdir1 = r_old_u*s + r_old_v*c;
	}
	pdir2 = new_norm.crossProduct(pdir1);
}

//Compute largest eigenvalue and associated engenvector of a symmetric 2*2 matrix.
//Solves chatacteristic equation.
//Inputs: three elements of matrix(upper-left,diag,lower-right)
//Outputs: largest (in magnitude) eigenvector/value
void largest_eig_2x2(float m1, float m12, float m2, vector2df& e1, float &l1)
{
	l1 = 0.5f*(m1 + m2);
	//The result of the below sqrt is positive, so to get the largest
	//eigenvalue we add it if we were positive already, else subtract
	if (l1>0.0f)
		l1 += sqrt(m12*m12 + 0.25f*(m2 - m1)*(m2 - m1));
	else
		l1 -= sqrt(m12*m12 + 0.25f*(m2 - m1)*(m2 - m1));

	//Find corresponding eigenvector
	e1 = vector2df(m2 - l1, -m12);
	e1.normalize();
}


//Compute principal view-dependent curvatures and directions at vertex i.
//ndotv=cosin of angle between normal and view direction.
//(u,v)=coordinates of w (projected view) in principal corrdinates
//Pass in u^2 , u*v, and v^2, since those are readily avaiable.
// Fills in q1 and t1 (using the paper's notation)
// Note that the latter is expressed in the (pdir1,pdir2) coordinate basis.
void Shape::compute_viewdep_curv(int i, float ndotv, float u2, float uv, float v2, float &q1, vector2df &t1)
{
	//Find the entries in Q=S*P^-1
	//                     =S+(sec theta-1)*S*w*w^T
	float sectheta_minus1 = 1.0f / fabs(ndotv) - 1.0f;
	float Q11, Q12, Q21, Q22;
	Q11 = curv1[i] * (1.0f + sectheta_minus1*u2);
	Q12 = curv1[i] * (sectheta_minus1*uv);
	Q21 = curv2[i] * (sectheta_minus1*uv);
	Q22 = curv2[i] * (1.0f + sectheta_minus1*v2);

	//Find the three entries in the (symmetric) matrix Q^T Q
	float QTQ1 = Q11*Q11 + Q21*Q21;
	float QTQ12 = Q11*Q12 + Q21*Q22;
	float QTQ2 = Q12*Q12 + Q22*Q22;

	//Computer eigenstuff
	largest_eig_2x2(QTQ1, QTQ12, QTQ2, t1, q1);


}

// Compute D_{t_1} q_1 - the derivative of max view-dependent curvature
// in the principal max view-dependent curvature direction
void Shape::compute_Dt1q1(int i, float ndotv, const vector<float> &q1, const vector<vector2df> &t1, float &Dt1q1)
{
	const vector3df &v0 = mObjects[0].mVerts[i];
	float this_viewdep_curv = q1[i];
	vector3df world_t1 = pdir1[i] * t1[i].X + pdir2[i] * t1[i].Y;
	vector3df world_t2 = normals[i].crossProduct(world_t1);
	float v0_dot_t2 = v0.dotProduct(world_t2);

	Dt1q1 = 0.0f;
	int n = 0;

	int naf = adjacentfaces[i].size();
	for (int j = 0; j<naf; j++)
	{
		//We are in a triangle adjacent to the vertex of interest;
		//The current vertex is v0 - let v1 and v2 be the other two
		int f = adjacentfaces[i][j];
		int ind = (mObjects[0].mFacets[f].indexOfPointA == i) ? 0 : (mObjects[0].mFacets[f].indexOfPointB == i ? 1 : (mObjects[0].mFacets[f].indexOfPointC == i ? 2 : -1));
		int i1, i2;
		if (ind == 0)
		{
			i1 = mObjects[0].mFacets[f].indexOfPointB;
			i2 = mObjects[0].mFacets[f].indexOfPointC;
		}
		else if (ind == 1)
		{
			i1 = mObjects[0].mFacets[f].indexOfPointC;
			i2 = mObjects[0].mFacets[f].indexOfPointA;
		}
		else
		{
			i1 = mObjects[0].mFacets[f].indexOfPointA;
			i2 = mObjects[0].mFacets[f].indexOfPointB;
		}

		const vector3df &v1 = mObjects[0].mVerts[i1];
		const vector3df &v2 = mObjects[0].mVerts[i2];

		//Find the point p on the segment between v1 and v2 such that its vector from v0 is along t1, i.e. perpendicular to t2.
		//Linear combination: p=w1*v1+w2*v2, where w2=1-w1
		float v1_dot_t2 = v1.dotProduct(world_t2);
		float v2_dot_t2 = v2.dotProduct(world_t2);
		float w1 = (v2_dot_t2 - v0_dot_t2) / (v2_dot_t2 - v1_dot_t2);

		//If w1 is not in [0,1) then we are not interested.
		//Incidentally, the computation of w1 can result in infinity,
		//but the comparsion should do the right thing..
		if (w1<0.0f || w1 >= 1.0f)
			continue;
		//Construct the opposite point
		float w2 = 1.0f - w1;
		vector3df p = v1*w1 + v2*w2;

		//And interpolate to find the view_dependent curvature at that point.
		float interp_viewdep_curv = w1*q1[i1] + w2*q1[i2];

		//Finally, take the *projected* view-dependent curvature derivative.
		float proj_dist = (p - v0).dotProduct(world_t1);
		proj_dist *= fabs(ndotv);
		Dt1q1 += (interp_viewdep_curv - this_viewdep_curv) / proj_dist;
		n++;

		//To save time, quit as soon as we have two estimates
		//(that is all we are going to get, anyway)
		if (n == 2)
		{
			Dt1q1 *= 0.5f;
			return;
		}
	}
}

//Draw part of an apparent ridge/valley curve on one triangle face.
//v0,v1,v2 are the indices of the 3 vertices: the function assumes that the curve connects points on the edges v0-v1 and v1-v2
//(or connects point on v0-v1 to center if to_center is true)
void Shape::draw_segment_app_ridge(int v0, int v1, int v2,
	float emax0, float emax1, float emax2,
	float kmax0, float kmax1, float kmax2,
	const vector3df &tmax0, const vector3df &tmax1, const vector3df& tmax2,
	float thresh, bool to_center, bool do_test)
{
	//Interpolate to find ridge/valley line segment endpoints.
	//in this triangle and the curvature there
	float w10 = fabs(emax0) / (fabs(emax0) + fabs(emax1));
	float w01 = 1.0f - w10;
	vector3df p01 = mObjects[0].mVerts[v0] * w01 + mObjects[0].mVerts[v1] * w10;
	float k01 = fabs(w01*kmax0 + w10*kmax1);

	vector3df p12;
	float k12;
	if (to_center)
	{
		//Connect the first point to center of triangle
		p12 = (mObjects[0].mVerts[v0] + mObjects[0].mVerts[v1] + mObjects[0].mVerts[v2]) / 3.0f;
		k12 = fabs(kmax0 + kmax1 + kmax2) / 3.0f;
	}
	else
	{
		//Connect first point to second one(on next edge)
		float w21 = fabs(emax1) / (fabs(emax1) + fabs(emax2));
		float w12 = 1.0f - w21;
		p12 = mObjects[0].mVerts[v1] * w12 + mObjects[0].mVerts[v2] * w21;
		k12 = fabs(w12*kmax1 + w21*kmax2);
	}

	//Do not draw below thresold
	k01 -= thresh;
	if (k01<0.0f)
		k01 = 0.0f;
	k12 -= thresh;
	if (k12<0.0f)
		k12 = 0.0f;

	//Skip lines that you can't see..
	if (k01 == 0.0f&&k12 == 0.0f)
		return;

	//Perform test: do the tmax-es point*towards* the segment?(Fig. 6)
	if (do_test)
	{
		//Find the vector perpendicular to the segment(p01 <-> p12)
		vector3df perp = ((mObjects[0].mVerts[v1] - mObjects[0].mVerts[v0]).crossProduct(mObjects[0].mVerts[v2] - mObjects[0].mVerts[v0])).crossProduct(p01 - p12)*0.5f;

		//We want tmax1 to point opposite to perp, and tmax0 and tmax2 to point along it. Otherwise, exit out.
		if (tmax0.dotProduct(perp) <= 0.0f || tmax1.dotProduct(perp) >= 0.0f || tmax2.dotProduct(perp) <= 0.0f)
			return;
	}

	//Fade lines
	if (draw_faded)
	{
		k01 /= (k01 + thresh);
		k12 /= (k12 + thresh);
	}
	else
		k01 = k12 = 1.0f;

	//Draw the line segment
	//fout<<v0<<" "<<v1<<" "<<v2<<" ("<<p01.X<<", "<<p01.Y<<", "<<p01.Z<<") ("<<p12.X<<", "<<p12.Y<<", "<<p12.Z<<")"<<endl;
	//glVertex3f(p01.X,p01.Y,p01.Z);
	//glVertex3f(p12.X,p12.Y,p12.Z);

	if (to_center)
	{
		addFaceSil(p01, p12, v0, v1, curFace);
	}
	else
	{
		addEdgeSil(p01, p12, v0, v1, v1, v2);
	}

}

//Draw apparent ridges in a triangle
void Shape::draw_face_app_ridges(int v0, int v1, int v2,
	const vector<float> &ndotv, const vector<float> &q1,
	const vector<vector2df> &t1, const vector<float>& Dt1q1,
	bool do_bfcull, bool do_test, float thresh)
{
#if 0
	//Backface culling is turned off: getting contours from the apparent ridge definition requires us to process faces that may be (just barely) backfacing..
	if (likely(do_bgcull&&ndotv[v0] <= 0.0f&&ndotv[v1] <= 0.0f&&nodtv[v2]<0.0f))
		return;
#endif

	//Trival reject if this face is not getting past the threshold anyway
	const float &kmax0 = q1[v0];
	const float &kmax1 = q1[v1];
	const float &kmax2 = q1[v2];
	if (kmax0 <= thresh&&kmax1 <= thresh&&kmax2 <= thresh)
		return;

	//The "tmax" are the principal directions of view-dependent curvature,
	//flipped to point in the direction in which the curvature is increasing

	const float &emax0 = Dt1q1[v0];
	const float &emax1 = Dt1q1[v1];
	const float &emax2 = Dt1q1[v2];
	vector3df world_t1_0 = pdir1[v0] * t1[v0].X + pdir2[v0] * t1[v0].Y;
	vector3df world_t1_1 = pdir1[v1] * t1[v1].X + pdir2[v1] * t1[v1].Y;
	vector3df world_t1_2 = pdir1[v2] * t1[v2].X + pdir2[v2] * t1[v2].Y;
	vector3df tmax0 = world_t1_0*Dt1q1[v0];
	vector3df tmax1 = world_t1_1*Dt1q1[v1];
	vector3df tmax2 = world_t1_2*Dt1q1[v2];

	//We have a "zero crossing" if the tmaxes along an edge
	//point in opposite directions
	bool z01 = (tmax0.dotProduct(tmax1) <= 0.0f);
	bool z12 = (tmax1.dotProduct(tmax2) <= 0.0f);
	bool z20 = (tmax2.dotProduct(tmax0) <= 0.0f);

	if (z01 + z12 + z20<2)
		return;

	//Draw line segment
	if (!z01)
	{
		draw_segment_app_ridge(v1, v2, v0, emax1, emax2, emax0, kmax1, kmax2, kmax0, tmax1, tmax2, tmax0, thresh, false, do_test);
	}
	else if (!z12)
	{
		draw_segment_app_ridge(v2, v0, v1, emax2, emax0, emax1, kmax2, kmax0, kmax1, tmax2, tmax0, tmax1, thresh, false, do_test);
	}
	else if (!z20)
	{
		draw_segment_app_ridge(v0, v1, v2, emax0, emax1, emax2, kmax0, kmax1, kmax2, tmax0, tmax1, tmax2, thresh, false, do_test);
	}
	else
	{
		//All three edges have crossings --- connect all to center
		draw_segment_app_ridge(v1, v2, v0, emax1, emax2, emax0, kmax1, kmax2, kmax0, tmax1, tmax2, tmax0, thresh, true, do_test);
		draw_segment_app_ridge(v2, v0, v1, emax2, emax0, emax1, kmax2, kmax0, kmax1, tmax2, tmax0, tmax1, thresh, true, do_test);
		draw_segment_app_ridge(v0, v1, v2, emax0, emax1, emax2, kmax0, kmax1, kmax2, tmax0, tmax1, tmax2, thresh, true, do_test);
	}
}

//Draw apparent ridges of the mesh
void Shape::draw_mesh_app_ridges(const vector<float> &ndotv, const vector<float>& q1,
	const vector<vector2df> &t1, const vector<float> &Dt1q1, bool do_bfcull, bool do_test, float thresh)
{
	for (int i = 0; i<mObjects[0].mFacets.size(); i++)
	{
		curFace = i;		//////////////////////////////////////////////////////////////////////////
		draw_face_app_ridges(mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointC,
			ndotv, q1, t1, Dt1q1, do_bfcull, do_test, thresh);
	}
}

//Compute per_vertex n dot l,n dot v ,radial curvature, and derivative of curvature for the current view
void Shape::compute_perview(vector<float> &ndotv, vector<float> &kr, vector<float> &sctest_num, vector<float> &sctest_den,
	vector<float> &shtest_num, vector<float> &q1, vector<vector2df> &t1, vector<float> &Dt1q1, bool extra_sin2theta)
{
	need_adjacentfaces();
	int nv = mObjects[0].mVerts.size();

	float scthresh = sug_thresh / (feature_size*feature_size);
	float shthresh = sh_thresh / (feature_size*feature_size);
	bool need_DwKr = (draw_sc || draw_sh || draw_DwKr);

	ndotv.resize(nv);
	kr.resize(nv);
	q1.resize(nv);
	t1.resize(nv);
	Dt1q1.resize(nv);

	if (need_DwKr)
	{
		sctest_num.resize(nv);
		sctest_den.resize(nv);
		if (draw_sh)
			shtest_num.resize(nv);
	}

	//Compute quantities at each vertex
	for (int i = 0; i<nv; i++)
	{
		//Compute n Dot v
		/*vector3df viewdir=viewpos-mObjects[0].mVerts[i];
		float r1v=1.0f/viewdir.getLength();
		viewdir*=r1v;*/
		ndotv[i] = viewVec.dotProduct(normals[i]);

		float u = viewVec.dotProduct(pdir1[i]), u2 = u*u;
		float v = viewVec.dotProduct(pdir2[i]), v2 = v*v;

		//Note: this is actually Kr*sin^2 theta
		kr[i] = curv1[i] * u2 + curv2[i] * v2;

		if (draw_apparent)
		{
			float csc2theta = 1.0f / (u2 + v2);
			compute_viewdep_curv(i, ndotv[i], u2*csc2theta, u*v*csc2theta, v2*csc2theta, q1[i], t1[i]);
		}

		if (!need_DwKr)
			continue;

		//Use DwKr*sin(theta)/cos(theta) for cutoff test
		sctest_num[i] = u2*(u*dcurv[i].X + 3.0f*v*dcurv[i].Y) + v2*(3.0f*u*dcurv[i].Z + v*dcurv[i].W);
		float csc2theta = 1.0f / (u2 + v2);
		sctest_num[i] *= csc2theta;
		float tr = (curv2[i] - curv1[i])*u*v*csc2theta;
		sctest_num[i] -= 2.0f*ndotv[i] * tr*tr;
		if (extra_sin2theta)
			sctest_num[i] *= u2 + v2;

		sctest_den[i] = ndotv[i];

		if (draw_sh)
		{
			sctest_num[i] -= sctest_num[i];
			shtest_num[i] -= shthresh*sctest_den[i];
		}
		sctest_num[i] -= scthresh*sctest_den[i];
	}

	if (draw_apparent)
	{
		for (int i = 0; i<nv; i++)
		{
			compute_Dt1q1(i, ndotv[i], q1, t1, Dt1q1[i]);
		}
	}
}
//Compute gradient of (kr*sin^2 theta) at vertex i
vector3df Shape::gradkr(int i)
{
	//vector3df viewdir=viewpos-mObjects[0].mVerts[i];
	//float rlen_viewdir=1.0f/viewdir.getLength();
	//viewdir*=rlen_viewdir;

	float ndotv = viewVec.dotProduct(normals[i]);
	float sintheta = sqrt(1.0f - ndotv*ndotv);
	float csctheta = 1.0f / sintheta;
	float u = (viewVec.dotProduct(pdir1[i]))*csctheta;
	float v = (viewVec.dotProduct(pdir2[i]))*csctheta;
	float kr = curv1[i] * u*u + curv2[i] * v*v;
	float tr = u*v*(curv2[i] - curv1[i]);
	float kt = curv1[i] * (1.0f - u*u) + curv2[i] * (1.0f - v*v);
	vector3df w = pdir1[i] * u + pdir2[i] * v;
	vector3df wperp = pdir2[i] * u - pdir1[i] * v;
	const vector4df &C = dcurv[i];
	vector3df g = pdir1[i] * (u*u*C.X + 2.0f*u*v*C.Y + v*v*C.Z) + pdir2[i] * (u*u*C.Y + 2.0f*u*v*C.Z + v*v*C.W) - ((w*tr + wperp*kt)*ndotv + wperp*1.0f)*2.0f*csctheta*tr;
	g *= (1.0f - ndotv*ndotv);
	g -= (w*kr + wperp*tr)*2.0f*kr*sintheta*ndotv;
	return g;
}

//Find a zero crossing using Hermite interpolation
float Shape::find_zero_hermite(int v0, int v1, float val0, float val1, const vector3df &grad0, const vector3df &grad1)
{
	if (unlikely(val0 == val1))
		return 0.5f;

	//Find derivatives along edge (of interpolation parameter in [0,1]
	//which means that e01 does not get normalized)

	vector3df e01 = mObjects[0].mVerts[v1] - mObjects[0].mVerts[v0];
	float d0 = e01.dotProduct(grad0), d1 = e01.dotProduct(grad1);

	//Coeffs of cubis a*s^3+b*s^2+c*s+d
	float a = 2 * (val0 - val1) + d0 + d1;
	float b = 3 * (val1 - val0) - 2 * d0 - d1;
	float c = d0, d = val0;

	//Find a root by bisection (as Newton can wander out of desired interval)
	//Start with entire [0,1] interval
	float sl = 0.0f, sr = 1.0f, valsl = val0, valsr = val1;

	//Check if we are in a (somewhat uncommon) 3-root situation, and pick the middle root if it happens (given we aren`t drawing curvy lines seems the best approach.
	//Finde extrema of derivative (a->3a;b->2b;c->c)
	//and check if they are both in [0,1] and have different signs.
	float disc = 4 * b - 12 * a*c;
	if (disc>0 && a != 0)
	{
		disc = sqrt(disc);
		float r1 = (-2 * b + disc) / (6 * a);
		float r2 = (-2 * b - disc) / (6 * a);
		if (r1 >= 0 && r1 <= 1 && r2 >= 0 && r2 <= 1)
		{
			float vr1 = (((a*r1 + b)*r1 + c)*r1) + d;
			float vr2 = (((a*r2 + b)*r2 + c)*r2) + d;
			// When extrema have different signs inside an
			// interval with endpoints with different signs,
			// the middle root is in between the two extrema
			if (vr1<0.0f&&vr2 >= 0.0f || vr1>0.0f&&vr2 <= 0.0f)
			{
				// 3 roots
				if (r1<r2)
				{
					sl = r1;
					valsl = vr1;
					sr = r2;
					valsr = vr2;
				}
				else
				{
					sl = r2;
					valsl = vr2;
					sr = r1;
					valsr = vr1;
				}
			}
		}
	}

	//Bisection method (constant number of interations)
	for (int iter = 0; iter<10; iter++)
	{
		float sbi = (sl + sr) / 2.0f;
		float valsbi = (((a*sbi + b)*sbi) + c)*sbi + d;

		// Keep the half which has different signs
		if (valsl<0.0f&&valsbi >= 0.0f || valsl>0.0f&&valsbi <= 0.0f)
		{
			sr = sbi;
			valsr = valsbi;
		}
		else
		{
			sl = sbi;
			valsl = valsbi;
		}
	}

	return 0.5f*(sl + sr);
}

//Draw part of a zero_crossing curve on one triangle face, but only if "test_num/test_den" is positive.
//v0,v1,v2 are the indices of the 3 vertices, "val" are the values of the scalar field whose zero crossings we are finding, and "test_*" are values we are testing to make sure they are positive.
//This function assumes that val0 has opposite sign from val1 and val2 - the following function is the general one that figures out which one actually has the different sign
void Shape::draw_face_isoline2(int v0, int v1, int v2, const std::vector<float> &val, const std::vector<float> &test_num, const std::vector<float> &test_den, bool do_hermite, bool do_test, float fade)
{
	//How far along each edge?
	float w10 = do_hermite ? find_zero_hermite(v0, v1, val[v0], val[v1], gradkr(v0), gradkr(v1)) : find_zero_linear(val[v0], val[v1]);
	float w01 = 1.0f - w10;
	float w20 = do_hermite ? find_zero_hermite(v0, v2, val[v0], val[v2], gradkr(v0), gradkr(v1)) : find_zero_linear(val[v0], val[v2]);

	float w02 = 1.0f - w20;


	//Points along edges
	vector3df p1 = mObjects[0].mVerts[v0] * w01 + mObjects[0].mVerts[v1] * w10;
	vector3df p2 = mObjects[0].mVerts[v0] * w02 + mObjects[0].mVerts[v2] * w20;

	addEdgeSil(p1, p2, v0, v1, v0, v2);

}


//See above.. This is the driver function that figures out which of v0,v1,v2 has a different sign from the others
void Shape::draw_face_isoline(int v0, int v1, int v2, const std::vector<float> &val, const std::vector<float> &test_num, const std::vector<float> &test_den, const std::vector<float> &ndotv, bool do_bfcull, bool do_hermite, bool do_test, float fade)
{
	//Backface culling
	if (likely(do_bfcull&&ndotv[v0] <= 0.0f&&ndotv[v1] <= 0.0f&&ndotv[v2] <= 0.0f))
		return;

	//Quick reject if derivs are negative
	if (do_test)
	{
		if (test_den.empty())
		{
			if (test_num[v0] <= 0.0f&&test_num[v1] <= 0.0f&&test_num[v2] <= 0.0f)
				return;
		}
		else
		{
			if (test_num[v0] <= 0.0f&&test_num[v1] <= 0.0f&&test_num[v2] <= 0.0f&&test_den[v0] >= 0.0f&&test_den[v1] >= 0.0f&&test_den[v2] >= 0.0f)
				return;
			if (test_num[v0] >= 0.0f&&test_num[v1] >= 0.0f&&test_num[v2] >= 0.0f&&test_den[v0] <= 0.0f&&test_den[v1] <= 0.0f&&test_den[v2] <= 0.0f)
				return;
		}
	}

	//Figure out which val has different sign, and draw
	if (val[v0]<0.0f&&val[v1] >= 0.0f&&val[v2] >= 0.0f || val[v0]>0.0f&&val[v1] <= 0.0f&&val[v2] <= 0.0f)
		draw_face_isoline2(v0, v1, v2, val, test_num, test_den, do_hermite, do_test, fade);
	else if (val[v1]<0.0f&&val[v2] >= 0.0f&&val[v0] >= 0.0f || val[v1]>0.0f&&val[v2] <= 0.0f&&val[v0] <= 0.0f)
		draw_face_isoline2(v1, v2, v0, val, test_num, test_den, do_hermite, do_test, fade);
	else if (val[v2]<0.0f&&val[v0] >= 0.0f&&val[v1] >= 0.0f || val[v2]>0.0f&&val[v0] <= 0.0f&&val[v1] <= 0.0f)
		draw_face_isoline2(v2, v0, v1, val, test_num, test_den, do_hermite, do_test, fade);
}

//Draw exterior silhouette of the mesh: this just draws thick contours, which are partially hidden by the mesh 
//Note: this needs to happen *before* draw_base_mesh
void Shape::draw_silhouette(const std::vector<float> &ndotv)
{

	draw_isolines(ndotv, vector<float>(), vector<float>(), ndotv, false, false, false, 0.0f);


	//Wide lines are gappy,so fill them in
	draw_isolines(ndotv, vector<float>(), vector<float>(), ndotv, false, false, false, 0.0f);

}

//Take a scalar field and renders the zero crossings, but only where test_num/test_den is greater than 0
void Shape::draw_isolines(const std::vector<float> &val, const std::vector<float> &test_num, const std::vector<float> &test_den, const std::vector<float> &ndotv, bool do_bfcull, bool do_hermite, bool do_test, float fade)
{
	for (int i = 0; i<mObjects[0].mFacets.size(); i++)
	{

		curFace = i;		//////////////////////////////////////////////////////////////////////////


		//Draw a line if ,among the values in this triangle,
		//at least one is positive and one is negative
		const float &v0 = val[mObjects[0].mFacets[i].indexOfPointA];
		const float &v1 = val[mObjects[0].mFacets[i].indexOfPointB];
		const float &v2 = val[mObjects[0].mFacets[i].indexOfPointC];

		if (unlikely((v0>0.0f || v1>0.0f || v2>0.0f) && (v0<0.0f || v1<0.0f || v2<0.0f)))
			draw_face_isoline(mObjects[0].mFacets[i].indexOfPointC, mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointA, val, test_num, test_den, ndotv, do_bfcull, do_hermite, do_test, fade);
	}
}

//Draw K=0,H=0, and DwKr=thresh line
void Shape::draw_misc(const std::vector<float> &ndotv, const std::vector<float> &DwKr, bool do_hidden)
{
	int nv = mObjects[0].mVerts.size();

	if (draw_K)
	{
		vector<float> K(nv);
		for (int i = 0; i<nv; i++)
			K[i] = curv1[i] * curv2[i];
		draw_isolines(K, vector<float>(), vector<float>(), ndotv, !do_hidden, false, false, 0.0f);
	}
	if (draw_H)
	{
		vector<float> H(nv);
		for (int i = 0; i<nv; i++)
		{
			H[i] = 0.5f*(curv1[i] + curv2[i]);
		}
		draw_isolines(H, vector<float>(), vector<float>(), ndotv, !do_hidden, false, false, 0.0f);
	}
	if (draw_DwKr)
	{
		draw_isolines(DwKr, vector<float>(), vector<float>(), ndotv, !do_hidden, false, false, 0.0f);
	}
}


//Draw part of a ridge/valley curve on one triangle face : v0,v1,v2 are the indices of the 3 vertices;
//this function assumes that the curve connects points on the edges v0-v1 and v1-v2 (or connects point on v0-v1 to center if to_center is true)
void Shape::draw_segment_ridge(int v0, int v1, int v2, float emax0, float emax1, float emax2, float kmax0, float kmax1, float kmax2, float thresh, bool to_center)
{
	//Interpolate to find ridge/valley line segment endpoints
	//in this triangle and the curvature there
	float w10 = fabs(emax0) / (fabs(emax0) + fabs(emax1));
	float w01 = 1.0f - w10;
	vector3df p01 = mObjects[0].mVerts[v0] * w01 + mObjects[0].mVerts[v1] * w10;
	float k01 = fabs(w01*kmax0 + w10*kmax1);

	vector3df p12;
	float k12;
	if (to_center)
	{
		//Connect first point to center of triangle
		p12 = (mObjects[0].mVerts[v0] + mObjects[0].mVerts[v1] + mObjects[0].mVerts[v2]) / 3.0f;
		k12 = fabs(kmax0 + kmax1 + kmax2) / 3.0f;
	}
	else
	{
		//Connect first point to second one(on next edge)
		float w21 = fabs(emax1) / (fabs(emax1) + fabs(emax2));
		float w12 = 1.0f - w21;
		p12 = mObjects[0].mVerts[v1] * w12 + mObjects[0].mVerts[v2] * w21;
		k12 = fabs(w12*kmax1 + w21*kmax2);
	}

	//Do not draw below threshold
	k01 -= thresh;
	if (k01<0.0f)
		k01 = 0.0f;
	k12 -= thresh;
	if (k12<0.0f)
		k12 = 0.0f;

	//Skip lines that you can`t see
	if (k01 == 0.0f&&k12 == 0.0f)
		return;

	//Fade lines
	if (draw_faded)
	{
		k01 /= (k01 + thresh);
		k12 /= (k12 + thresh);
	}
	else
	{
		k01 = k12 = 1.0f;
	}

	//Draw the line segment

	if (to_center)
	{
		addFaceSil(p01, p12, v0, v1, curFace);
	}
	else
	{
		addEdgeSil(p01, p12, v0, v1, v1, v2);
	}
}

//Draw ridges or valleys(depending on do_ridge) in a triangle v0,v1,v2 
// -uses ndotv for backface culling (enabled with do_bfcull)
// -do_test checks for curvature maxima/minina for ridges/valleys
// (when off, it draws positive minima and negative maxima)
// Note: this computes ridges/valley every time, instead of once at the start (given they are not view dependent, this is wasteful)
// Algorithm based on formulas of ohtake et al . 2004
void Shape::draw_face_ridges(int v0, int v1, int v2, bool do_ridge, const vector<float> &ndotv, bool do_bfcull, bool do_test, float thresh)
{
	//Backface culling
	if (likely(do_bfcull&&ndotv[v0] <= 0.0f&&ndotv[v1] <= 0.0f&&ndotv[v2] <= 0.0f))
		return;

	//Check if ridge possible at vertices just based on curvature
	if (do_ridge)
	{
		if (curv1[v0] <= 0.0f || curv1[v1] <= 0.0f || curv1[v2] <= 0.0f)
			return;
	}
	else
	{
		if (curv1[v0] >= 0.0f || curv1[v1] >= 0.0f || curv1[v2] >= 0.0f)
			return;
	}

	//Sign of curvature on ridge/valley
	float rv_sign = do_ridge ? 1.0f : -1.0f;

	//The "tmax" are the principal directions of maximal curvature, flipped to point in the direction in which the curvature is increasing(decreasing for valleys).
	//Note that this is a bit different from the notation in Ohtake et al., but the tests below are equivalent.
	const float &emax0 = dcurv[v0].X;
	const float &emax1 = dcurv[v1].X;
	const float &emax2 = dcurv[v2].X;
	vector3df tmax0 = pdir1[v0] * rv_sign*dcurv[v0].X;
	vector3df tmax1 = pdir1[v1] * rv_sign*dcurv[v1].X;
	vector3df tmax2 = pdir1[v2] * rv_sign*dcurv[v2].X;

	//We have a "zero crossing" if the tmaxes along an edge point in opposite directions
	bool z01 = (tmax0.dotProduct(tmax1) <= 0.0f);
	bool z12 = (tmax1.dotProduct(tmax2) <= 0.0f);
	bool z20 = (tmax2.dotProduct(tmax0) <= 0.0f);

	if (z01 + z12 + z20<2)
		return;

	if (do_test)
	{
		const vector3df &p0 = mObjects[0].mVerts[v0], &p1 = mObjects[0].mVerts[v1], &p2 = mObjects[0].mVerts[v2];

		//Check whether we have the correct flavor of extremum: Is the curvature increasing along the edge?
		z01 = z01 && ((tmax0.dotProduct(p1 - p0)) >= 0.0f || (tmax1.dotProduct(p1 - p0) <= 0.0f));
		z12 = z12 && ((tmax1.dotProduct(p2 - p1)) >= 0.0f || (tmax2.dotProduct(p2 - p1) <= 0.0f));
		z20 = z20 && ((tmax2.dotProduct(p0 - p2)) >= 0.0f || (tmax0.dotProduct(p0 - p2) <= 0.0f));

		if (z01 + z12 + z20<2)
			return;
	}

	//Draw line segment
	const float &kmax0 = curv1[v0];
	const float &kmax1 = curv1[v1];
	const float &kmax2 = curv1[v2];

	if (!z01)
	{
		draw_segment_ridge(v1, v2, v0, emax1, emax2, emax0, kmax1, kmax2, kmax0, thresh, false);
	}
	else if (!z12)
	{
		draw_segment_ridge(v2, v0, v1, emax2, emax0, emax1, kmax2, kmax0, kmax1, thresh, false);
	}
	else if (!z20)
	{
		draw_segment_ridge(v0, v1, v2, emax0, emax1, emax2, kmax0, kmax1, kmax2, thresh, false);
	}
	else
	{
		//All three edges have crossings -- connect all to center
		draw_segment_ridge(v1, v2, v0, emax1, emax2, emax0, kmax1, kmax2, kmax0, thresh, true);
		draw_segment_ridge(v2, v0, v1, emax2, emax0, emax1, kmax2, kmax0, kmax1, thresh, true);
		draw_segment_ridge(v0, v1, v2, emax0, emax1, emax2, kmax0, kmax1, kmax2, thresh, true);
	}

}

//Draw the ridges (valleys) of the mesh
void Shape::draw_mesh_ridges(bool do_ridge, const std::vector<float> &ndotv, bool do_bfcull, bool do_test, float thresh)
{
	for (int i = 0; i<mObjects[0].mFacets.size(); i++)
	{
		curFace = i;		//////////////////////////////////////////////////////////////////////////
		draw_face_ridges(mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointC, do_ridge, ndotv, do_bfcull, do_test, thresh);
	}
}


//Draw principal highlights on a face
void Shape::draw_face_ph(int v0, int v1, int v2, bool do_ridge, const std::vector<float> &ndotv, bool do_bfcull, bool do_test, float thresh)
{
	//Backface culling
	if (likely(do_bfcull&&ndotv[v0] <= 0.0f&&ndotv[v1] <= 0.0f&&ndotv[v2] <= 0.0f))
		return;

	//Orient principal directions based on the largest principal curvature
	float k0 = curv1[v0];
	float k1 = curv1[v1];
	float k2 = curv1[v2];

	if (do_test&&do_ridge&&min(min(k0, k1), k2)<0.0f)
		return;
	if (do_test&&do_ridge&&max(max(k1, k0), k2)>0.0f)
		return;

	vector3df d0 = pdir1[v0];
	vector3df d1 = pdir1[v1];
	vector3df d2 = pdir1[v2];

	float kmax = fabs(k0);

	//dref is the e1 vector with the largest |k1|
	vector3df dref = d0;
	if (fabs(k1)>kmax)
		kmax = fabs(k1), dref = d1;
	if (fabs(k2)>kmax)
		kmax = fabs(k2), dref = d2;

	//Flip all the e1 to agree with dref
	if (d0.dotProduct(dref)<0.0f)
		d0 = -d0;
	if (d1.dotProduct(dref)<0.0f)
		d1 = -d1;
	if (d2.dotProduct(dref)<0.0f)
		d2 = -d2;

	//If directions have flipped (more than 45 degree), then give up
	if ((d0.dotProduct(dref)<sqrt(0.5f)) || (d1.dotProduct(dref)<sqrt(0.5f)) || (d2.dotProduct(dref)<sqrt(0.5f)))
		return;

	/*//Compute view dirctions, dot products at each vertex
	vector3df viewdir0=viewpos-mObjects[0].mVerts[v0];
	vector3df viewdir1=viewpos-mObjects[0].mVerts[v1];
	vector3df viewdir2=viewpos-mObjects[0].mVerts[v2];

	//Normalize those for cos(theta) later
	viewdir0.normalize();
	viewdir1.normalize();
	viewdir2.normalize();
	*/
	//e1 dot w sin(theta)
	//---which is zero when looking down e2
	float dot0 = viewVec.dotProduct(d0);
	float dot1 = viewVec.dotProduct(d1);
	float dot2 = viewVec.dotProduct(d2);

	//We have a "zero crossing" if the dot products along an edge
	//have opposite signs
	int z01 = (dot0*dot1 <= 0.0f);
	int z12 = (dot1*dot2 <= 0.0f);
	int z20 = (dot2*dot0 <= 0.0f);

	if (z01 + z12 + z20<2)
		return;

	//Draw line segment
	float test0 = (curv1[v0] * curv1[v0] - curv2[v0] * curv2[v0])*viewVec.dotProduct(normals[v0]);
	float test1 = (curv1[v1] * curv1[v1] - curv2[v1] * curv2[v1])*viewVec.dotProduct(normals[v1]);
	float test2 = (curv1[v2] * curv1[v2] - curv2[v2] * curv2[v2])*viewVec.dotProduct(normals[v2]);

	if (!z01)
	{
		draw_segment_ridge(v1, v2, v0, dot1, dot2, dot0, test1, test2, test0, thresh, false);
	}
	else if (!z12)
	{
		draw_segment_ridge(v2, v0, v1, dot2, dot0, dot1, test2, test0, test1, thresh, false);
	}
	else if (!z20)
	{
		draw_segment_ridge(v0, v1, v2, dot0, dot1, dot2, test0, test1, test2, thresh, false);
	}
}

//Draw principal highlights
void Shape::draw_mesh_ph(bool do_ridge, const std::vector<float> &ndotv, bool do_bfcull, bool do_test, float thresh)
{
	for (int i = 0; i<mObjects[0].mFacets.size(); i++)
	{
		curFace = i;		//////////////////////////////////////////////////////////////////////////
		draw_face_ph(mObjects[0].mFacets[i].indexOfPointA, mObjects[0].mFacets[i].indexOfPointB, mObjects[0].mFacets[i].indexOfPointC, do_ridge, ndotv, do_bfcull, do_test, thresh);
	}
}


//Draw the boundaries on the mesh
void Shape::draw_boundaries(bool do_hidden)
{
	need_across_edge();

	if (do_hidden)
	{
		glColor3f(0.6, 0.6, 0.6);
		glLineWidth(1.5);
	}
	else
	{
		glColor3f(0.05, 0.05, 0.05);
		glLineWidth(2.5);
	}

	glBegin(GL_LINES);
	for (int i = 0; i<mObjects[0].mFacets.size(); i++)
	{

		if (across_edge[i].indexOfPointA<0)
		{
			int v1 = mObjects[0].mFacets[i].indexOfPointB;
			int v2 = mObjects[0].mFacets[i].indexOfPointC;
			glVertex3f(mObjects[0].mVerts[v1].X, mObjects[0].mVerts[v1].Y, mObjects[0].mVerts[v1].Z);
			glVertex3f(mObjects[0].mVerts[v2].X, mObjects[0].mVerts[v2].Y, mObjects[0].mVerts[v2].Z);
		}
		if (across_edge[i].indexOfPointB<0)
		{
			int v1 = mObjects[0].mFacets[i].indexOfPointC;
			int v2 = mObjects[0].mFacets[i].indexOfPointA;
			glVertex3f(mObjects[0].mVerts[v1].X, mObjects[0].mVerts[v1].Y, mObjects[0].mVerts[v1].Z);
			glVertex3f(mObjects[0].mVerts[v2].X, mObjects[0].mVerts[v2].Y, mObjects[0].mVerts[v2].Z);
		}
		if (across_edge[i].indexOfPointC<0)
		{
			int v1 = mObjects[0].mFacets[i].indexOfPointA;
			int v2 = mObjects[0].mFacets[i].indexOfPointB;
			glVertex3f(mObjects[0].mVerts[v1].X, mObjects[0].mVerts[v1].Y, mObjects[0].mVerts[v1].Z);
			glVertex3f(mObjects[0].mVerts[v2].X, mObjects[0].mVerts[v2].Y, mObjects[0].mVerts[v2].Z);
		}
	}
	glEnd();
}

//Draw lines of n.l=const
void Shape::draw_isophotes(const vector<float> &ndotv)
{

}

//Draw lines of constant depth
void Shape::draw_topolines(const std::vector<float> &ndotv)
{

}
//Compute a "feature size" for the mesh: computed as 1% of the reciprocal of the 10-th percentile curvature
void Shape::computer_feature_size()
{
	int nv = mObjects[0].mVerts.size();
	int nsamp = min(nv, 500);

	vector<float> samples;
	samples.reserve(nsamp * 2);

	for (int i = 0; i<nsamp; i++)
	{
		//Quick'n dirty portable random number generator
		static unsigned randq = 0;
		randq = unsigned(1664525)*randq + unsigned(1013904223);

		int ind = randq%nv;
		samples.push_back(fabs(curv1[ind]));
		samples.push_back(fabs(curv2[ind]));

	}

	const float frac = 0.1f;
	const float mult = 0.01f;
	need_bsphere();

	float max_feature_size = 0.05f*bsphere.r;

	int which = int(frac*samples.size());
	nth_element(samples.begin(), samples.begin() + which, samples.end());

	feature_size = min(mult / samples[which], max_feature_size);
}

//Find axis_aligned bounding box of the vertices
void Shape::need_bbox()
{
	if (mObjects[0].mVerts.empty() || bbox.valid)
		return;
	bbox.min = bbox.max = mObjects[0].mVerts[0];
	for (int i = 1; i<mObjects[0].mVerts.size(); i++)
	{
		if (mObjects[0].mVerts[i].X<bbox.min.X) bbox.min.X = mObjects[0].mVerts[i].X;
		if (mObjects[0].mVerts[i].Y<bbox.min.Y) bbox.min.Y = mObjects[0].mVerts[i].Y;
		if (mObjects[0].mVerts[i].Z<bbox.min.Z) bbox.min.Z = mObjects[0].mVerts[i].Z;
		if (mObjects[0].mVerts[i].X>bbox.max.X) bbox.max.X = mObjects[0].mVerts[i].X;
		if (mObjects[0].mVerts[i].X>bbox.max.Y) bbox.max.Y = mObjects[0].mVerts[i].Y;
		if (mObjects[0].mVerts[i].X>bbox.max.Z) bbox.max.Z = mObjects[0].mVerts[i].Z;
	}
	bbox.valid = true;
}


//Find extreme vertex in a given direction
int Shape::farthest_vertex_along(const vector3df& dir)
{
	const vector<mVert> &v = mObjects[0].mVerts;
	int nv = v.size();
	int farthest = 0;
	float farthest_dot = v[0].dotProduct(dir);

	for (int i = 1; i<nv; i++)
	{
		float my_dot = v[i].dotProduct(dir);
		if (my_dot>farthest_dot)
		{
			farthest = i;
			farthest_dot = my_dot;
		}
	}
	return farthest;
}

//Approximate bounding sphere code based on an algrithm by Ritter
void Shape::need_bsphere()
{
	if (mObjects[0].mVerts.empty() || bsphere.valid)
		return;
	need_bbox();

	vector3df best_min, best_max;
	vector<vector3df> dirs;
	dirs.push_back(vector3df(1, 0, 0));
	dirs.push_back(vector3df(0, 1, 0));
	dirs.push_back(vector3df(0, 0, 1));
	dirs.push_back(vector3df(1, 1, 1));
	dirs.push_back(vector3df(1, -1, 1));
	dirs.push_back(vector3df(1, -1, -1));
	dirs.push_back(vector3df(1, 1, -1));

	for (int i = 0; i<dirs.size(); i++)
	{
		vector3df p1 = mObjects[0].mVerts[farthest_vertex_along(-dirs[i])];
		vector3df p2 = mObjects[0].mVerts[farthest_vertex_along(dirs[i])];
		if ((p2 - p1).getLengthSQ()>(best_min - best_max).getLengthSQ())
		{
			best_min = p1;
			best_max = p2;
		}
	}
	bsphere.center = (best_min + best_max) / 2.0f;
	bsphere.r = (best_max - best_min).getLength() / 2.0;
	float r2 = bsphere.r*bsphere.r;

	//Expand bsphere to contain all points
	for (int i = 0; i<mObjects[0].mVerts.size(); i++)
	{
		float d2 = (mObjects[0].mVerts[i] - bsphere.center).getLengthSQ();
		if (d2 <= r2)
			continue;
		float d = sqrt(d2);
		bsphere.r = (bsphere.r + d) / 2.0f;
		r2 = bsphere.r*bsphere.r;
		bsphere.center -= mObjects[0].mVerts[i];
		bsphere.center *= bsphere.r / d;
		bsphere.center += mObjects[0].mVerts[i];
	}
	bsphere.valid = true;
}

//compute the normal of every vertex
void Shape::need_normals()
{
	int nf = this->mObjects[0].mFacets.size();
	int nv = this->mObjects[0].mVerts.size();
	normals.clear();
	normals.resize(nv);
	if (nf != 0)
	{
		//Find normals of a mesh
		for (int i = 0; i<nf; i++)
		{
			vector3df p0 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointA];
			vector3df p1 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointB];
			vector3df p2 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointC];

			vector3df a = p0 - p1;
			vector3df b = p1 - p2;
			vector3df c = p2 - p0;
			float l2a = a.getLengthSQ();
			float l2b = b.getLengthSQ();
			float l2c = c.getLengthSQ();

			vector3df facenormal = a.crossProduct(b);
			normals[mObjects[0].mFacets[i].indexOfPointA] += facenormal*(1.0f / (l2a*l2c));
			normals[mObjects[0].mFacets[i].indexOfPointB] += facenormal*(1.0f / (l2b*l2a));
			normals[mObjects[0].mFacets[i].indexOfPointC] += facenormal*(1.0f / (l2c*l2b));
		}
	}
	else
	{
		cout << "the number of faces is 0" << endl;
		exit(-1);
	}
	for (int i = 0; i<nv; i++)
	{
		normals[i].normalize();
	}
}

void Shape::need_pointareas()
{
	int nf = this->mObjects[0].mFacets.size();
	int nv = this->mObjects[0].mVerts.size();
	pointareas.clear();
	pointareas.resize(nv);
	cornerareas.clear();
	cornerareas.resize(nf);

	for (int i = 0; i<nf; i++)
	{
		//Edges
		vector3df p0 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointA];
		vector3df p1 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointB];
		vector3df p2 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointC];
		vector3df e[3] = { p2 - p1, p0 - p2, p1 - p0 };

		//Computer corner weights
		float area = 0.5f*e[0].crossProduct(e[1]).getLength();
		float l2[3] = { e[0].getLengthSQ(), e[1].getLengthSQ(), e[2].getLengthSQ() };
		float ew[3] = { l2[0] * (l2[1] + l2[2] - l2[0]), l2[1] * (l2[2] + l2[0] - l2[1]), l2[2] * (l2[0] + l2[1] - l2[2]) };
		if (ew[0] <= 0.0f)
		{
			cornerareas[i].Y = -0.25f*l2[2] * area / (e[0].dotProduct(e[2]));
			cornerareas[i].Z = -0.25f*l2[1] * area / (e[0].dotProduct(e[1]));
			cornerareas[i].X = area - cornerareas[i].Y - cornerareas[i].Z;
		}
		else if (ew[1] <= 0.0f)
		{
			cornerareas[i].Z = -0.25f*l2[0] * area / (e[1].dotProduct(e[0]));
			cornerareas[i].X = -0.25f*l2[2] * area / (e[1].dotProduct(e[2]));
			cornerareas[i].Y = area - cornerareas[i].Z - cornerareas[i].X;
		}
		else if (ew[2] <= 0.0f)
		{
			cornerareas[i].X = -0.25f*l2[1] * area / (e[2].dotProduct(e[1]));
			cornerareas[i].Y = -0.25f*l2[0] * area / (e[2].dotProduct(e[0]));
			cornerareas[i].Z = area - cornerareas[i].X - cornerareas[i].Y;
		}
		else
		{
			float ewscale = 0.5f*area / (ew[0] + ew[1] + ew[2]);
			cornerareas[i].X = ewscale*(ew[(0 + 1) % 3] + ew[(0 + 2) % 3]);
			cornerareas[i].Y = ewscale*(ew[(1 + 1) % 3] + ew[(1 + 2) % 3]);
			cornerareas[i].Z = ewscale*(ew[(2 + 1) % 3] + ew[(2 + 2) % 3]);
		}
		pointareas[mObjects[0].mFacets[i].indexOfPointA] += cornerareas[i].X;
		pointareas[mObjects[0].mFacets[i].indexOfPointB] += cornerareas[i].Y;
		pointareas[mObjects[0].mFacets[i].indexOfPointC] += cornerareas[i].Z;
	}

}
void Shape::need_curvatures()
{
	need_normals();
	need_pointareas();

	//Reszie the arrays we'll be using
	int nf = this->mObjects[0].mFacets.size();
	int nv = this->mObjects[0].mVerts.size();

	curv1.clear();
	curv2.clear();
	pdir1.clear();
	pdir2.clear();
	curv1.resize(nv);
	curv2.resize(nv);
	pdir1.resize(nv);
	pdir2.resize(nv);

	vector<float> curv12(nv);

	//Set up an initial coordinate system per vertex
	for (int i = 0; i<nf; i++)
	{
		pdir1[mObjects[0].mFacets[i].indexOfPointA] = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointB] - mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointA];
		pdir1[mObjects[0].mFacets[i].indexOfPointB] = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointC] - mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointB];
		pdir1[mObjects[0].mFacets[i].indexOfPointC] = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointA] - mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointC];
	}

	for (int i = 0; i<nv; i++)
	{
		pdir1[i] = pdir1[i].crossProduct(normals[i]);
		pdir1[i].normalize();
		pdir2[i] = normals[i].crossProduct(pdir1[i]);
	}

	//Compute curvature per-face
	for (int i = 0; i<nf; i++)
	{
		//Edges
		vector3df p0 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointA];
		vector3df p1 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointB];
		vector3df p2 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointC];
		vector3df e[3] = { p2 - p1, p0 - p2, p1 - p0 };

		//N-T-B coordinate system per face
		vector3df t = e[0];
		t.normalize();
		vector3df n = e[0].crossProduct(e[1]);
		vector3df b = n.crossProduct(t);
		b.normalize();

		//Estimate curvature based on variation of normals
		//along egdes
		float m[3] = { 0, 0, 0 };
		float w[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
		for (int j = 0; j<3; j++)
		{
			float u = e[j].dotProduct(t);
			float v = e[j].dotProduct(b);
			w[0][0] += u*u;
			w[0][1] += u*v;
			w[2][2] += v*v;
			vector3df dn;
			if (j == 0)
			{
				dn = normals[mObjects[0].mFacets[i].indexOfPointC] - normals[mObjects[0].mFacets[i].indexOfPointB];
			}
			else if (j == 1)
			{
				dn = normals[mObjects[0].mFacets[i].indexOfPointA] - normals[mObjects[0].mFacets[i].indexOfPointC];
			}
			else
			{
				dn = normals[mObjects[0].mFacets[i].indexOfPointB] - normals[mObjects[0].mFacets[i].indexOfPointA];
			}
			float dnu = dn.dotProduct(t);
			float dnv = dn.dotProduct(b);
			m[0] += dnu*u;
			m[1] += dnu*v + dnv*u;
			m[2] += dnv*v;
		}
		w[1][1] = w[0][0] + w[2][2];
		w[1][2] = w[0][1];

		//Least squares solution
		float diag[3];
		if (!ldltdc<float, 3>(w, diag))
		{
			continue;
		}
		ldltsl<float, 3>(w, diag, m, m);

		//Push it back out to the vertices
		for (int j = 0; j<3; j++)
		{
			int vj;
			if (j == 0)
			{
				vj = mObjects[0].mFacets[i].indexOfPointA;
			}
			else if (j == 1)
			{
				vj = mObjects[0].mFacets[i].indexOfPointB;
			}
			else
			{
				vj = mObjects[0].mFacets[i].indexOfPointC;
			}

			float c1, c12, c2;
			proj_curv(t, b, m[0], m[1], m[2], pdir1[vj], pdir2[vj], c1, c12, c2);
			float wt;
			if (j == 0)
			{
				wt = cornerareas[i].X / pointareas[vj];
			}
			else if (j == 1)
			{
				wt = cornerareas[i].Y / pointareas[vj];
			}
			else
			{
				wt = cornerareas[i].Z / pointareas[vj];
			}
			curv1[vj] += wt*c1;
			curv12[vj] += wt*c12;
			curv2[vj] += wt*c2;
		}
	}
	for (int i = 0; i<nv; i++)
	{
		diagonalize_curv(pdir1[i], pdir2[i], curv1[i], curv12[i], curv2[i], normals[i], pdir1[i], pdir2[i], curv1[i], curv2[i]);
	}
}

void Shape::need_dcurv()
{
	need_curvatures();

	//Reszie the arrays we'll be using
	int nf = this->mObjects[0].mFacets.size();
	int nv = this->mObjects[0].mVerts.size();

	dcurv.clear();
	dcurv.resize(nv);

	//compute dcurv per-face
	for (int i = 0; i<nf; i++)
	{
		//Edges
		vector3df p0 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointA];
		vector3df p1 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointB];
		vector3df p2 = mObjects[0].mVerts[mObjects[0].mFacets[i].indexOfPointC];
		vector3df e[3] = { p2 - p1, p0 - p2, p1 - p0 };

		//N-T-B coordinate system per face
		vector3df t = e[0];
		t.normalize();
		vector3df n = e[0].crossProduct(e[1]);
		vector3df b = n.crossProduct(t);
		b.normalize();

		//Project curvature tensor from each vertex into this
		//face 's coordinate system
		vector3df fcurv[3];
		int vj = mObjects[0].mFacets[i].indexOfPointA;
		float c1, c2, c3;
		proj_curv(pdir1[vj], pdir2[vj], curv1[vj], 0, curv2[vj], t, b, c1, c2, c3);
		fcurv[0].set(c1, c2, c3);
		vj = mObjects[0].mFacets[i].indexOfPointB;
		proj_curv(pdir1[vj], pdir2[vj], curv1[vj], 0, curv2[vj], t, b, c1, c2, c3);
		fcurv[1].set(c1, c2, c3);
		vj = mObjects[0].mFacets[i].indexOfPointC;
		proj_curv(pdir1[vj], pdir2[vj], curv1[vj], 0, curv2[vj], t, b, c1, c2, c3);
		fcurv[2].set(c1, c2, c3);

		//Estimate dcurv based on variation of curvature along edges
		float m[4] = { 0, 0, 0, 0 };
		float w[4][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
		for (int j = 0; j<3; j++)
		{
			//Variation of curvature of along each edge
			vector3df dfcurv = fcurv[(j - 1 + 3) % 3] - fcurv[(j + 1) % 3];
			float u = e[j].dotProduct(t);
			float v = e[j].dotProduct(b);
			float u2 = u*u, v2 = v*v, uv = u*v;
			w[0][0] += u2;
			w[0][1] += uv;
			w[3][3] += v2;
			m[0] += u*dfcurv.X;
			m[1] += v*dfcurv.X + 2.0f*u*dfcurv.Y;
			m[2] += 2.0f*v*dfcurv.Y + u*dfcurv.Z;
			m[3] += v*dfcurv.Z;
		}
		w[1][1] = 2.0f*w[0][0] + w[3][3];
		w[1][2] = 2.0f*w[0][1];
		w[2][2] = w[0][0] + 2.0f*w[3][3];
		w[2][3] = w[0][1];

		//Least squares solution
		float d[4];
		if (!ldltdc<float, 4>(w, d))
		{
			continue;
		}
		ldltsl<float, 4>(w, d, m, m);
		vector4df face_dcurv(m[0], m[1], m[2], m[3]);

		//Push it back out to the vertices
		for (int j = 0; j<3; j++)
		{
			int vj;
			if (j == 0)
			{
				vj = mObjects[0].mFacets[i].indexOfPointA;
			}
			else if (j == 1)
			{
				vj = mObjects[0].mFacets[i].indexOfPointB;
			}
			else
			{
				vj = mObjects[0].mFacets[i].indexOfPointC;
			}
			vector4df this_vert_dcurv;

			proj_dcurv(t, b, face_dcurv, pdir1[vj], pdir2[vj], this_vert_dcurv);
			float wt;
			if (j == 0)
			{
				wt = cornerareas[i].X / pointareas[vj];
			}
			else if (j == 1)
			{
				wt = cornerareas[i].Y / pointareas[vj];
			}
			else
			{
				wt = cornerareas[i].Z / pointareas[vj];
			}
			dcurv[vj] += this_vert_dcurv*wt;
		}
	}
}

//Find the direct neighbours of each vertex
void Shape::need_neighbours()
{
	int nf = this->mObjects[0].mFacets.size();
	int nv = this->mObjects[0].mVerts.size();

	vector<int> numneighbours(nv);
	for (int i = 0; i<nf; i++)
	{
		numneighbours[mObjects[0].mFacets[i].indexOfPointA]++;
		numneighbours[mObjects[0].mFacets[i].indexOfPointB]++;
		numneighbours[mObjects[0].mFacets[i].indexOfPointC]++;
	}

	neighbour.clear();
	neighbour.resize(nv);
	for (int i = 0; i<nv; i++)
	{
		neighbour[i].reserve(numneighbours[i] + 2);
	}

	for (int i = 0; i<nf; i++)
	{
		//vector<int> me;
		int n1, n2;

		vector<int> &me = neighbour[mObjects[0].mFacets[i].indexOfPointA];
		n1 = mObjects[0].mFacets[i].indexOfPointB;
		n2 = mObjects[0].mFacets[i].indexOfPointC;
		if (find(me.begin(), me.end(), n1) == me.end())
			me.push_back(n1);
		if (find(me.begin(), me.end(), n2) == me.end())
			me.push_back(n2);

		vector<int> &me1 = neighbour[mObjects[0].mFacets[i].indexOfPointB];
		n1 = mObjects[0].mFacets[i].indexOfPointC;
		n2 = mObjects[0].mFacets[i].indexOfPointA;
		if (find(me1.begin(), me1.end(), n1) == me1.end())
			me1.push_back(n1);
		if (find(me1.begin(), me1.end(), n2) == me1.end())
			me1.push_back(n2);

		vector<int> &me2 = neighbour[mObjects[0].mFacets[i].indexOfPointC];
		n1 = mObjects[0].mFacets[i].indexOfPointA;
		n2 = mObjects[0].mFacets[i].indexOfPointB;
		if (find(me2.begin(), me2.end(), n1) == me2.end())
			me2.push_back(n1);
		if (find(me2.begin(), me2.end(), n2) == me2.end())
			me2.push_back(n2);

	}
}

//Find the faces touching each vertex
void Shape::need_adjacentfaces()
{
	int nf = this->mObjects[0].mFacets.size();
	int nv = this->mObjects[0].mVerts.size();

	vector<int> numadjacentfaces(nv);
	for (int i = 0; i<nf; i++)
	{
		numadjacentfaces[mObjects[0].mFacets[i].indexOfPointA]++;
		numadjacentfaces[mObjects[0].mFacets[i].indexOfPointB]++;
		numadjacentfaces[mObjects[0].mFacets[i].indexOfPointC]++;
	}

	adjacentfaces.clear();
	adjacentfaces.resize(nv);
	for (int i = 0; i<nv; i++)
		adjacentfaces[i].reserve(numadjacentfaces[i]);

	for (int i = 0; i<nf; i++)
	{
		adjacentfaces[mObjects[0].mFacets[i].indexOfPointA].push_back(i);
		adjacentfaces[mObjects[0].mFacets[i].indexOfPointB].push_back(i);
		adjacentfaces[mObjects[0].mFacets[i].indexOfPointC].push_back(i);
	}
}

//Find the face across each edge from each other face(-1 on boundary)
//If topology is bad, not necessarily what one would expect
void Shape::need_across_edge()
{
	need_adjacentfaces();

	int nf = this->mObjects[0].mFacets.size();
	int nv = this->mObjects[0].mVerts.size();

	mFacet face;
	face.indexOfPointA = face.indexOfPointB = face.indexOfPointC = -1;
	across_edge.resize(nf, face);

	for (int i = 0; i<nf; i++)
	{
		if (across_edge[i].indexOfPointA == -1)
		{
			int v1 = mObjects[0].mFacets[i].indexOfPointB;
			int v2 = mObjects[0].mFacets[i].indexOfPointC;
			const vector<int> &a1 = adjacentfaces[v1];
			const vector<int> &a2 = adjacentfaces[v2];
			for (int k1 = 0; k1<a1.size(); k1++)
			{
				int other = a1[k1];
				if (other == i)
					continue;
				vector<int>::const_iterator it = find(a2.begin(), a2.end(), other);
				if (it == a2.end())
					continue;
				int ind = (mObjects[0].mFacets[other].indexOfPointA == v1) ? 0 : (mObjects[0].mFacets[other].indexOfPointB == v1 ? 1 : (mObjects[0].mFacets[other].indexOfPointC == v1 ? 2 : -1));
				ind = (ind + 1) % 3;
				if (ind == 0 && mObjects[0].mFacets[other].indexOfPointB != v2)
					continue;
				else if (ind == 1 && mObjects[0].mFacets[other].indexOfPointC != v2)
					continue;
				else if (ind == 2 && mObjects[0].mFacets[other].indexOfPointA != v2)
					continue;
				across_edge[i].indexOfPointA = other;
				if (ind == 0)
					across_edge[i].indexOfPointA = i;
				else if (ind == 1)
					across_edge[i].indexOfPointB = i;
				else
					across_edge[i].indexOfPointC = i;
				break;
			}
		}

		if (across_edge[i].indexOfPointB == -1)
		{
			int v1 = mObjects[0].mFacets[i].indexOfPointC;
			int v2 = mObjects[0].mFacets[i].indexOfPointA;
			const vector<int> &a1 = adjacentfaces[v1];
			const vector<int> &a2 = adjacentfaces[v2];
			for (int k1 = 0; k1<a1.size(); k1++)
			{
				int other = a1[k1];
				if (other == i)
					continue;
				vector<int>::const_iterator it = find(a2.begin(), a2.end(), other);
				if (it == a2.end())
					continue;
				int ind = (mObjects[0].mFacets[other].indexOfPointA == v1) ? 0 : (mObjects[0].mFacets[other].indexOfPointB == v1 ? 1 : (mObjects[0].mFacets[other].indexOfPointC == v1 ? 2 : -1));
				ind = (ind + 1) % 3;
				if (ind == 0 && mObjects[0].mFacets[other].indexOfPointB != v2)
					continue;
				else if (ind == 1 && mObjects[0].mFacets[other].indexOfPointC != v2)
					continue;
				else if (ind == 2 && mObjects[0].mFacets[other].indexOfPointA != v2)
					continue;
				across_edge[i].indexOfPointB = other;
				if (ind == 0)
					across_edge[i].indexOfPointA = i;
				else if (ind == 1)
					across_edge[i].indexOfPointB = i;
				else
					across_edge[i].indexOfPointC = i;
				break;
			}
		}

		if (across_edge[i].indexOfPointC == -1)
		{
			int v1 = mObjects[0].mFacets[i].indexOfPointA;
			int v2 = mObjects[0].mFacets[i].indexOfPointB;
			const vector<int> &a1 = adjacentfaces[v1];
			const vector<int> &a2 = adjacentfaces[v2];
			for (int k1 = 0; k1<a1.size(); k1++)
			{
				int other = a1[k1];
				if (other == i)
					continue;
				vector<int>::const_iterator it = find(a2.begin(), a2.end(), other);
				if (it == a2.end())
					continue;
				int ind = (mObjects[0].mFacets[other].indexOfPointA == v1) ? 0 : (mObjects[0].mFacets[other].indexOfPointB == v1 ? 1 : (mObjects[0].mFacets[other].indexOfPointC == v1 ? 2 : -1));
				ind = (ind + 1) % 3;
				if (ind == 0 && mObjects[0].mFacets[other].indexOfPointB != v2)
					continue;
				else if (ind == 1 && mObjects[0].mFacets[other].indexOfPointC != v2)
					continue;
				else if (ind == 2 && mObjects[0].mFacets[other].indexOfPointA != v2)
					continue;
				across_edge[i].indexOfPointC = other;
				if (ind == 0)
					across_edge[i].indexOfPointA = i;
				else if (ind == 1)
					across_edge[i].indexOfPointB = i;
				else
					across_edge[i].indexOfPointC = i;
				break;
			}
		}
	}
}

void Shape::setViewvec()
{
	GLdouble M[16], P[16];
	GLint V[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, M);
	glGetDoublev(GL_PROJECTION_MATRIX, P);
	glGetIntegerv(GL_VIEWPORT, V);
	GLdouble v1[3], v2[3];
	gluUnProject(0, 0, 0, M, P, V, &v1[0], &v1[1], &v1[2]);
	gluUnProject(0, 0, 1, M, P, V, &v2[0], &v2[1], &v2[2]);
	viewVec.set(v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]);
	viewVec.normalize();
}
//////////////////////////////////////////////////////////////////////////



void Image::init()
{
	ready = false;
	width = 700; height = 700;
	width3D = 700; height3D = 700;
	scale = scale3D = 1.0;
	anglex3D = angley3D = anglez3D = 0.0;
	colorSil = false;
	myShape.init();
	myShape.update();

	isHandle = new int[myShape.nPoint];

}

void Image::drawShape()
{
	myShape.draw();
}

void Image::bigger()
{
	scale3D += 0.1;
}
void Image::smaller()
{
	scale3D -= 0.1;
	if (scale3D < 0)
		scale3D = 0;
}
void Image::turnLeft()
{
	angley3D -= 10;
	if (angley3D < 0)
		angley3D += 360;
}
void Image::turnRight()
{
	angley3D += 10;
	if (angley3D > 360)
		angley3D -= 360;
}
void Image::turnUp()
{
	anglex3D -= 10;
	if (anglex3D < 0)
		anglex3D += 360;
}
void Image::turnDown()
{
	anglex3D += 10;
	if (anglex3D > 360)
		anglex3D -= 360;
}

void Image::turnS()			//r
{
	anglez3D -= 10;
	if (anglez3D < 0)
		anglez3D += 360;
}

void Image::turnN()			//l
{
	anglez3D += 10;
	if (anglez3D > 360)
		anglez3D -= 360;
}


void Image::getViewVector()
{
	vector3df v1, v2;
	transToObjCood(0, 0, 0, v1.X, v1.Y, v1.Z);
	transToObjCood(0, 0, 1, v2.X, v2.Y, v2.Z);
	viewVec = v2 - v1;
}

void Image::transToObjCood(double x, double y, double z, double &tx, double &ty, double &tz)
{
	GLdouble ox, oy, oz;
	gluUnProject((GLdouble)x, (GLdouble)y, z, modelMatrix, projMatrix, viewport, &ox, &oy, &oz);
	tx = ox; ty = oy; tz = oz;
}

void Image::transToWinCood(double x, double y, double z, double &tx, double &ty, double &tz)
{
	gluProject(x, y, z, modelMatrix, projMatrix, viewport, &tx, &ty, &tz);
}

Image::~Image()
{
	delete[]isHandle;
}

void Image::resetStroke()
{
	defStroke.clear();
	defStroke3D.clear();
	matchSil3D.clear();
	reDefStroke3D.clear();
}


bool Image::borderCan(int i, int j)
{
	if (i < 0 || i >= height)
		return false;
	if (j < 0 || j >= width)
		return false;
	return true;
}

void Image::dfsTrace(vector<int> &line, int i)
{
	vector2di v;
	int mj;
	double mind, tmp;

	set<int>::iterator it;

	traced[i] = true;
	line.push_back(i);

	mj = -1;
	mind = InfDouble;
	for (it = myShape.adjSilPoint[i].begin(); it != myShape.adjSilPoint[i].end(); it++)
	{
		if (!traced[*it] && visable[*it])
		{
			tmp = fabs(silPointDepth[*it] - silPointDepth[i]);
			if (tmp < mind)
			{
				mind = tmp;
				mj = *it;
			}

		}
	}
	if (mj != -1)
		dfsTrace(line, mj);
}

void Image::tracing()
{
	int i, k, n;
	vector<int> tmp, line;


	traced.clear();
	for (i = 0; i < myShape.nSilPoint; ++i)
		traced.push_back(false);
	sil.clear();

	for (i = 0; i < myShape.nSilPoint; ++i)
	{
		if (!traced[i] && visable[i])
		{
			line.clear();

			tmp.clear();
			dfsTrace(tmp, i);
			n = tmp.size();
			for (k = n - 1; k >= 0; --k)
				line.push_back(tmp[k]);

			tmp.clear();
			dfsTrace(tmp, i);
			n = tmp.size();
			for (k = 1; k < n; ++k)				//k = 0 重复
				line.push_back(tmp[k]);

			sil.push_back(line);
		}
	}

	nSil = sil.size();

}


void Image::drawSil()
{
	if (sil.empty())
		return;
	int i, j;
	int n;

	glColor3f(0, 0, 0);
	glLineWidth(1.0);
	for (i = 0; i < nSil; ++i)
	{
		n = sil[i].size();

		if (colorSil)
		{
			double r, g, b;
			r = rand() % 1000 / 1000.0;
			g = rand() % 1000 / 1000.0;;
			b = rand() % 1000 / 1000.0;
			glColor3f(r, g, b);
		}

		glBegin(GL_LINE_STRIP);
		for (j = 0; j < n; ++j)
		{
			glVertex3f(myShape.silPoints[sil[i][j]].X, myShape.silPoints[sil[i][j]].Y, myShape.silPoints[sil[i][j]].Z);
		}
		if (cycle[i])
			glVertex3f(myShape.silPoints[sil[i][0]].X, myShape.silPoints[sil[i][0]].Y, myShape.silPoints[sil[i][0]].Z);
		glEnd();
	}


}

void Image::drawStroke(vector<vector3df> &stroke)
{
	if (stroke.empty() || stroke.size() < 2)
		return;

	int i, n;

	n = stroke.size();
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < n; ++i)
		glVertex3f(stroke[i].X, stroke[i].Y, stroke[i].Z);
	glEnd();
}

void Image::transToWinCood3D(vector3df p, vector3df &win)
{
	gluProject(p.X, p.Y, p.Z, modelMatrix3D, projMatrix3D, viewport3D, &win.X, &win.Y, &win.Z);
}

double Image::checkDepth(vector3df win)
{
	int i;
	int dx, dy;
	double mind;

	mind = 0;
	for (i = 0; i < 8; ++i)
	{
		dx = win.X + dir[i][0];
		dy = win.Y + dir[i][1];
		if (borderCan(dx, dy) && depthMap[dy*width3D + dx] > mind)
			mind = depthMap[dy*width3D + dx];
	}
	return mind;
}



void Image::checkVisable()
{
	int i;
	vector3df tmp;

	visable.clear();
	visable = vector<bool>(myShape.nSilPoint);
	for (i = 0; i < myShape.nSilPoint; ++i)
		visable[i] = true;


	/*深度缓存估计可见性*/
	//glutSetWindow(window3D);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix3D);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix3D);
	glGetIntegerv(GL_VIEWPORT, viewport3D);

	glReadPixels(0, 0, width3D, height3D, GL_DEPTH_COMPONENT, GL_FLOAT, depthMap);

	silPointDepth.clear();
	for (i = 0; i < myShape.nSilPoint; ++i)
	{
		transToWinCood3D(myShape.silPoints[i], tmp);
		silPointDepth.push_back(tmp.Z);
		if (tmp.Z - checkDepth(tmp) > depthEp)
			visable[i] = false;
	}

	//glutSetWindow(window2D);


}

void Image::checkEdges()
{
	int i, np;
	myShape.edgeDetection();
	checkVisable();
	tracing();
	cycle.clear();
	for (i = 0; i < nSil; ++i)
	{
		np = sil[i].size();
		cycle.push_back(false);
		if (myShape.adjSilPoint[sil[i][0]].find(sil[i][np - 1]) != myShape.adjSilPoint[sil[i][0]].end())
			cycle[i] = true;
	}


}

void Image::matchRefStroke()
{

	int i, j, k, nStr, np;
	int mid, midk, othermid;
	double totLen, len, lenother;
	vector2df vec;
	vector<vector2df> line;
	vector3df pos;

	double tmp, minDis;
	int minSil;
	int interIndex[2], pk[2], pj[2], tk[2], tIndex[2];
	vector2df interpoint[2];
	double mind, tmpd;


	if (defStroke.size() < 2)
		return;


	silPoint2D.clear();
	for (i = 0; i < myShape.nSilPoint; ++i)
	{
		transToWinCood(myShape.silPoints[i].X, myShape.silPoints[i].Y, myShape.silPoints[i].Z,
			vec.X, vec.Y, tmp);
		silPoint2D.push_back(vec);
	}


	nStr = defStroke.size();
	totLen = 0;
	for (i = 1; i < nStr; ++i)
		totLen += distance(defStroke[i - 1], defStroke[i]);

	totLen /= 2.0;
	len = 0;
	for (i = 1; i < nStr; ++i)
	{
		len += distance(defStroke[i - 1], defStroke[i]);
		if (len > totLen)
			break;
	}
	mid = i;


	nStr = defStroke.size();
	minDis = InfDouble;
	minSil = -1;

	for (i = 0; i < nSil; ++i)
	{
		np = sil[i].size();
		tIndex[0] = 1;
		for (j = 1; j <= mid; ++j)
		{
			line2d<double> l1(defStroke[j], defStroke[j - 1]);
			for (k = 1; k < np; ++k)
			{
				line2d<double> l2(silPoint2D[sil[i][k]], silPoint2D[sil[i][k - 1]]);
				if (intersect(l1, l2))
				{
					intersection(l1, l2, interpoint[0]);
					tIndex[0] = -1;
					tk[0] = k;
					pj[0] = j;
					break;
				}

			}
			if (tIndex[0] == -1)
				break;
		}


		tIndex[1] = 1;
		for (j = nStr - 1; j > mid; --j)
		{
			line2d<double> l1(defStroke[j], defStroke[j - 1]);
			for (k = 1; k < np; ++k)
			{
				line2d<double> l2(silPoint2D[sil[i][k]], silPoint2D[sil[i][k - 1]]);
				if (intersect(l1, l2))
				{
					intersection(l1, l2, interpoint[1]);
					tIndex[1] = -1;
					tk[1] = k;
					pj[1] = j;
					break;
				}

			}
			if (tIndex[1] == -1)
				break;
		}

		tmp = 0;

		if (tIndex[0] == -1)
			tmp += distance(interpoint[0], defStroke[0]);
		else
		{
			mind = InfDouble;
			for (k = 0; k < np; ++k)
			{
				tmpd = distance(silPoint2D[sil[i][k]], defStroke[0]);
				if (tmpd < mind)
				{
					mind = tmpd;
					tIndex[0] = sil[i][k];
					tk[0] = k;
				}
			}
			tmp += mind;
		}


		if (tIndex[1] == -1)
			tmp += distance(interpoint[1], defStroke[nStr - 1]);
		else
		{
			mind = InfDouble;
			for (k = 0; k < np; ++k)
			{
				tmpd = distance(silPoint2D[sil[i][k]], defStroke[nStr - 1]);
				if (tmpd < mind)
				{
					mind = tmpd;
					tIndex[1] = sil[i][k];
					tk[1] = k;
				}
			}
			tmp += mind;
		}

		if (tmp < minDis)
		{
			minDis = tmp;
			minSil = i;
			pk[0] = tk[0];
			pk[1] = tk[1];
			interIndex[0] = tIndex[0];
			interIndex[1] = tIndex[1];
		}
	}


	np = sil[minSil].size();
	line.clear();
	matchSil3D.clear();
	matchSil.clear();
	matchSilIndex.clear();
	if (interIndex[0] == -1)
	{
		if (interIndex[1] == -1)			//0，1都交点
		{
			line.push_back(interpoint[0]);
			for (j = pj[0]; j < pj[1]; ++j)
				line.push_back(defStroke[j]);
			line.push_back(interpoint[1]);


			len = 1; lenother = 10;
			if (cycle[minSil])
			{
				midk = (pk[0] + pk[1]) / 2;
				othermid = (midk + np / 2) % np;
				len = distance2(silPoint2D[sil[minSil][midk]], defStroke[mid]);
				lenother = distance2(silPoint2D[sil[minSil][othermid]], defStroke[mid]);
			}

			if (len < lenother)
			{
				if (pk[0] < pk[1])
				{
					getPoint3D(sil[minSil][pk[0] - 1], sil[minSil][pk[0]],
						distance(interpoint[0], silPoint2D[sil[minSil][pk[0] - 1]]) / distance(silPoint2D[sil[minSil][pk[0] - 1]], silPoint2D[sil[minSil][pk[0]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[0]);
					matchSilIndex.push_back(-1);
					for (k = pk[0]; k < pk[1]; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}

					getPoint3D(sil[minSil][pk[1] - 1], sil[minSil][pk[1]],
						distance(interpoint[1], silPoint2D[sil[minSil][pk[1] - 1]]) / distance(silPoint2D[sil[minSil][pk[1] - 1]], silPoint2D[sil[minSil][pk[1]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[1]);
					matchSilIndex.push_back(-1);
				}
				else
				{
					getPoint3D(sil[minSil][pk[0] - 1], sil[minSil][pk[0]],
						distance(interpoint[0], silPoint2D[sil[minSil][pk[0] - 1]]) / distance(silPoint2D[sil[minSil][pk[0] - 1]], silPoint2D[sil[minSil][pk[0]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[0]);
					matchSilIndex.push_back(-1);
					for (k = pk[0] - 1; k >= pk[1]; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}

					getPoint3D(sil[minSil][pk[1] - 1], sil[minSil][pk[1]],
						distance(interpoint[1], silPoint2D[sil[minSil][pk[1] - 1]]) / distance(silPoint2D[sil[minSil][pk[1] - 1]], silPoint2D[sil[minSil][pk[1]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[1]);
					matchSilIndex.push_back(-1);
				}
			}
			else
			{
				if (pk[0] < pk[1])
				{
					getPoint3D(sil[minSil][pk[0] - 1], sil[minSil][pk[0]],
						distance(interpoint[0], silPoint2D[sil[minSil][pk[0] - 1]]) / distance(silPoint2D[sil[minSil][pk[0] - 1]], silPoint2D[sil[minSil][pk[0]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[0]);
					matchSilIndex.push_back(-1);
					for (k = pk[0] - 1; k >= 0; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					for (k = np - 1; k >= pk[1]; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}

					getPoint3D(sil[minSil][pk[1] - 1], sil[minSil][pk[1]],
						distance(interpoint[1], silPoint2D[sil[minSil][pk[1] - 1]]) / distance(silPoint2D[sil[minSil][pk[1] - 1]], silPoint2D[sil[minSil][pk[1]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[1]);
					matchSilIndex.push_back(-1);
				}
				else
				{
					getPoint3D(sil[minSil][pk[0] - 1], sil[minSil][pk[0]],
						distance(interpoint[0], silPoint2D[sil[minSil][pk[0] - 1]]) / distance(silPoint2D[sil[minSil][pk[0] - 1]], silPoint2D[sil[minSil][pk[0]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[0]);
					matchSilIndex.push_back(-1);
					for (k = pk[0]; k < np; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					for (k = 0; k < pk[1]; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}

					getPoint3D(sil[minSil][pk[1] - 1], sil[minSil][pk[1]],
						distance(interpoint[1], silPoint2D[sil[minSil][pk[1] - 1]]) / distance(silPoint2D[sil[minSil][pk[1] - 1]], silPoint2D[sil[minSil][pk[1]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[1]);
					matchSilIndex.push_back(-1);
				}
			}

		}
		else							//0交，1不交
		{
			line.push_back(interpoint[0]);
			for (j = pj[0]; j < nStr; ++j)
				line.push_back(defStroke[j]);


			len = 1; lenother = 10;
			if (cycle[minSil])
			{
				midk = (pk[0] + pk[1]) / 2;
				othermid = (midk + np / 2) % np;
				len = distance2(silPoint2D[sil[minSil][midk]], defStroke[mid]);
				lenother = distance2(silPoint2D[sil[minSil][othermid]], defStroke[mid]);
			}

			if (len < lenother)
			{
				if (pk[0] < pk[1])
				{
					getPoint3D(sil[minSil][pk[0] - 1], sil[minSil][pk[0]],
						distance(interpoint[0], silPoint2D[sil[minSil][pk[0] - 1]]) / distance(silPoint2D[sil[minSil][pk[0] - 1]], silPoint2D[sil[minSil][pk[0]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[0]);
					matchSilIndex.push_back(-1);
					for (k = pk[0]; k <= pk[1]; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
				}
				else
				{
					getPoint3D(sil[minSil][pk[0] - 1], sil[minSil][pk[0]],
						distance(interpoint[0], silPoint2D[sil[minSil][pk[0] - 1]]) / distance(silPoint2D[sil[minSil][pk[0] - 1]], silPoint2D[sil[minSil][pk[0]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[0]);
					matchSilIndex.push_back(-1);
					for (k = pk[0] - 1; k >= pk[1]; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
				}
			}
			else
			{
				if (pk[0] < pk[1])
				{
					getPoint3D(sil[minSil][pk[0] - 1], sil[minSil][pk[0]],
						distance(interpoint[0], silPoint2D[sil[minSil][pk[0] - 1]]) / distance(silPoint2D[sil[minSil][pk[0] - 1]], silPoint2D[sil[minSil][pk[0]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[0]);
					matchSilIndex.push_back(-1);
					for (k = pk[0] - 1; k >= 0; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					for (k = np - 1; k >= pk[1]; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
				}
				else
				{
					getPoint3D(sil[minSil][pk[0] - 1], sil[minSil][pk[0]],
						distance(interpoint[0], silPoint2D[sil[minSil][pk[0] - 1]]) / distance(silPoint2D[sil[minSil][pk[0] - 1]], silPoint2D[sil[minSil][pk[0]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[0]);
					matchSilIndex.push_back(-1);
					for (k = pk[0]; k < np; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					for (k = 0; k <= pk[1]; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
				}
			}
		}
	}
	else
	{
		if (interIndex[1] == -1)			//0不交，1交
		{
			for (j = 0; j <= pj[1]; ++j)
				line.push_back(defStroke[j]);
			line.push_back(interpoint[1]);


			len = 1; lenother = 10;
			if (cycle[minSil])
			{
				midk = (pk[0] + pk[1]) / 2;
				othermid = (midk + np / 2) % np;
				len = distance2(silPoint2D[sil[minSil][midk]], defStroke[mid]);
				lenother = distance2(silPoint2D[sil[minSil][othermid]], defStroke[mid]);
			}

			if (len < lenother)
			{
				if (pk[0] < pk[1])
				{
					for (k = pk[0]; k < pk[1]; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					getPoint3D(sil[minSil][pk[1] - 1], sil[minSil][pk[1]],
						distance(interpoint[1], silPoint2D[sil[minSil][pk[1] - 1]]) / distance(silPoint2D[sil[minSil][pk[1] - 1]], silPoint2D[sil[minSil][pk[1]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[1]);
					matchSilIndex.push_back(-1);
				}
				else
				{
					for (k = pk[0]; k >= pk[1]; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					getPoint3D(sil[minSil][pk[1] - 1], sil[minSil][pk[1]],
						distance(interpoint[1], silPoint2D[sil[minSil][pk[1] - 1]]) / distance(silPoint2D[sil[minSil][pk[1] - 1]], silPoint2D[sil[minSil][pk[1]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[1]);
					matchSilIndex.push_back(-1);
				}
			}
			else
			{
				if (pk[0] < pk[1])
				{
					for (k = pk[0]; k >= 0; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					for (k = np - 1; k >= pk[1]; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					getPoint3D(sil[minSil][pk[1] - 1], sil[minSil][pk[1]],
						distance(interpoint[1], silPoint2D[sil[minSil][pk[1] - 1]]) / distance(silPoint2D[sil[minSil][pk[1] - 1]], silPoint2D[sil[minSil][pk[1]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[1]);
					matchSilIndex.push_back(-1);
				}
				else
				{
					for (k = pk[0]; k < np; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					for (k = 0; k < pk[1]; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					getPoint3D(sil[minSil][pk[1] - 1], sil[minSil][pk[1]],
						distance(interpoint[1], silPoint2D[sil[minSil][pk[1] - 1]]) / distance(silPoint2D[sil[minSil][pk[1] - 1]], silPoint2D[sil[minSil][pk[1]]]), pos);
					matchSil3D.push_back(pos);
					matchSil.push_back(interpoint[1]);
					matchSilIndex.push_back(-1);
				}
			}
		}
		else							//0，1都不交
		{
			for (j = 0; j < nStr; ++j)
				line.push_back(defStroke[j]);


			len = 1; lenother = 10;
			if (cycle[minSil])
			{
				midk = (pk[0] + pk[1]) / 2;
				othermid = (midk + np / 2) % np;
				len = distance2(silPoint2D[sil[minSil][midk]], defStroke[mid]);
				lenother = distance2(silPoint2D[sil[minSil][othermid]], defStroke[mid]);
			}

			if (len < lenother)
			{
				if (pk[0] < pk[1])
				{
					for (k = pk[0]; k <= pk[1]; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
				}
				else
				{
					for (k = pk[0]; k >= pk[1]; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
				}
			}
			else
			{
				if (pk[0] < pk[1])
				{
					for (k = pk[0]; k >= 0; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					for (k = np - 1; k >= pk[1]; --k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
				}
				else
				{
					for (k = pk[0]; k < np; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
					for (k = 0; k <= pk[1]; ++k)
					{
						matchSil3D.push_back(myShape.silPoints[sil[minSil][k]]);
						matchSil.push_back(silPoint2D[sil[minSil][k]]);
						matchSilIndex.push_back(sil[minSil][k]);
					}
				}
			}

		}
	}

	defStroke.clear();
	defStroke3D.clear();
	nStr = line.size();
	for (k = 0; k < nStr; ++k)
		defStroke.push_back(line[k]);
	for (k = 0; k < nStr; ++k)
	{
		transToObjCood(defStroke[k].X, defStroke[k].Y, 0, pos.X, pos.Y, pos.Z);
		defStroke3D.push_back(pos);
	}

}

void Image::getPoint3D(int i, int j, double r, vector3df &ans)
{
	ans = (myShape.silPoints[j] - myShape.silPoints[i]) * r + myShape.silPoints[i];
}

double Image::distance(vector2di v1, vector2di v2)
{
	return sqrt(double((v1.X - v2.X)*(v1.X - v2.X) + (v1.Y - v2.Y)*(v1.Y - v2.Y)));
}

int Image::distance2(vector2di v1, vector2di v2)
{
	return (v1.X - v2.X)*(v1.X - v2.X) + (v1.Y - v2.Y)*(v1.Y - v2.Y);
}

double Image::distance(vector2df p1, vector2df p2)
{
	return sqrt(double((p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y)));
}

double Image::distance2(vector2df p1, vector2df p2)
{
	return (p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y);
}


double Image::distance(vector3df p1, vector3df p2)
{
	return sqrt((p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y) + (p1.Z - p2.Z)*(p1.Z - p2.Z));
}

double Image::distance2(vector3df p1, vector3df p2)
{
	return (p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y) + (p1.Z - p2.Z)*(p1.Z - p2.Z);
}


vector3df Image::insert(vector3df s, vector3df e, double len)
{
	vector3df ans;
	double tot = distance(s, e);

	if (tot < len)
		return e;

	len /= tot;
	ans.X = len * (e.X - s.X) + s.X;
	ans.Y = len * (e.Y - s.Y) + s.Y;
	ans.Z = len * (e.Z - s.Z) + s.Z;

	return ans;
}


void Image::reconStroke()								//将目标线划分成等比例于匹配轮廓上的点
{
	int i, j;
	int nMatch, nDef;
	double totMatch, totDef, sumDef, sumMatch;
	vector3df curPoint, sDef, sMatch, matchPoint, p1, p2;
	vector<vector2df> tmp;
	double depth;


	nMatch = matchSil.size();
	nDef = defStroke.size();



	sumDef = distance2(matchSil[0], defStroke[0]) + distance2(matchSil[nMatch - 1], defStroke[nDef - 1]);
	sumMatch = distance2(matchSil[0], defStroke[nDef - 1]) + distance2(matchSil[nMatch - 1], defStroke[0]);


	if (sumDef > sumMatch)
	{
		tmp.clear();
		for (i = nDef - 1; i >= 0; --i)
			tmp.push_back(defStroke[i]);
		defStroke.clear();
		for (i = 0; i < nDef; ++i)
			defStroke.push_back(tmp[i]);
	}



	totMatch = 0.0;
	for (i = 1; i < nMatch; ++i)
	{
		totMatch += distance(matchSil[i - 1], matchSil[i]);
	}

	totDef = 0.0;
	for (i = 1; i < nDef; ++i)
	{
		totDef += distance(defStroke[i - 1], defStroke[i]);
	}

	rate = totMatch / totDef;

	reDefStroke3D.clear();

	sDef.X = defStroke[0].X;
	sDef.Y = defStroke[0].Y;
	sDef.Z = 0;
	reDefStroke3D.push_back(sDef);

	sMatch.X = matchSil[0].X;
	sMatch.Y = matchSil[0].Y;
	sMatch.Z = 0;

	i = j = 1;
	curPoint.Z = 0;
	matchPoint.Z = 0;

	curPoint.X = defStroke[1].X;
	curPoint.Y = defStroke[1].Y;

	matchPoint.X = matchSil[1].X;
	matchPoint.Y = matchSil[1].Y;
	while (i < nMatch && j < nDef)
	{
		sumDef = distance(sDef, curPoint) * rate;
		sumMatch = distance(sMatch, matchPoint);

		if (sumDef < sumMatch)
		{
			sDef = curPoint;
			sMatch = insert(sMatch, matchPoint, sumDef);
			++j;
			if (j >= nDef)
				break;
			curPoint.X = defStroke[j].X;
			curPoint.Y = defStroke[j].Y;
		}
		else
		{
			sDef = insert(sDef, curPoint, sumMatch / rate);
			reDefStroke3D.push_back(sDef);
			sMatch = matchPoint;
			++i;
			if (i >= nMatch)
				break;
			matchPoint.X = matchSil[i].X;
			matchPoint.Y = matchSil[i].Y;
		}
	}
	if (reDefStroke3D.size() < nMatch)
	{
		curPoint.X = defStroke[nDef - 1].X;
		curPoint.Y = defStroke[nDef - 1].Y;
		reDefStroke3D.push_back(curPoint);
	}

	transToWinCood(matchSil3D[0].X, matchSil3D[0].Y, matchSil3D[0].Z, curPoint.X, curPoint.Y, curPoint.Z);
	depth = curPoint.Z;
	transToWinCood(matchSil3D[nMatch - 1].X, matchSil3D[nMatch - 1].Y, matchSil3D[nMatch - 1].Z, curPoint.X, curPoint.Y, curPoint.Z);
	depth += curPoint.Z;
	depth /= 2.0;




	for (i = 0; i < nMatch; ++i)
	{
		transToObjCood(reDefStroke3D[i].X, reDefStroke3D[i].Y, depth, reDefStroke3D[i].X, reDefStroke3D[i].Y, reDefStroke3D[i].Z);

	}


	defStroke3D.clear();

}


void Image::changePos()					//handlePoint
{
	int i, j;
	vector<double> vecb;
	Matrix tmp;
	LinearSolver ls(myShape.nPoint);

	mMatrix = myShape.adjMatrix;

	j = myShape.nPoint;
	for (i = 0; i < nHandle; ++i, ++j)
	{
		mMatrix.insert(j, handlePoint[i], 1);
	}
	mMatrix.setStructure(j, myShape.nPoint);
	mMatrix.trans(mtMatrix);													//mtMatrix = MT
	mMatrix.transMulti(MTM);

	ls.setMatrixToA(MTM);


	//计算X
	solveMatrix.reset();
	for (i = 0; i < myShape.nPoint; ++i)
		solveMatrix.insert(i, 0, myShape.points[i].X);
	solveMatrix.setStructure(myShape.nPoint, 1);											//solveMatrix = (x1,...xn) = X
	myShape.adjMatrix.multi(solveMatrix, tmp);											//tmp = M * X	
	tmp.getRows(myShape.nPoint, solveMatrix);
	for (i = 0; i < nHandle; ++i)
		solveMatrix.insert(i + myShape.nPoint, 0, corrPos[i].X);
	solveMatrix.setStructure(myShape.nPoint + nHandle, 1);
	mtMatrix.multi(solveMatrix, tmp);

	vecb.clear();
	tmp.changeToVec(vecb);

	ls.setVectorToB(vecb);
	ls.solve(true);

	for (i = 0; i < myShape.nPoint; ++i)
		myShape.points[i].X = ls.x[i];



	//计算Y
	solveMatrix.reset();
	for (i = 0; i < myShape.nPoint; ++i)
		solveMatrix.insert(i, 0, myShape.points[i].Y);
	solveMatrix.setStructure(myShape.nPoint, 1);											//solveMatrix = (x1,...xn) = X
	myShape.adjMatrix.multi(solveMatrix, tmp);											//tmp = M * X	
	tmp.getRows(myShape.nPoint, solveMatrix);
	for (i = 0; i < nHandle; ++i)
		solveMatrix.insert(i + myShape.nPoint, 0, corrPos[i].Y);
	solveMatrix.setStructure(myShape.nPoint + nHandle, 1);
	mtMatrix.multi(solveMatrix, tmp);

	vecb.clear();
	tmp.changeToVec(vecb);

	ls.resetBX();
	ls.setVectorToB(vecb);
	ls.solve(true);

	for (i = 0; i < myShape.nPoint; ++i)
		myShape.points[i].Y = ls.x[i];



	//计算Z
	solveMatrix.reset();
	for (i = 0; i < myShape.nPoint; ++i)
		solveMatrix.insert(i, 0, myShape.points[i].Z);
	solveMatrix.setStructure(myShape.nPoint, 1);											//solveMatrix = (x1,...xn) = X
	myShape.adjMatrix.multi(solveMatrix, tmp);											//tmp = M * X	
	tmp.getRows(myShape.nPoint, solveMatrix);
	for (i = 0; i < nHandle; ++i)
		solveMatrix.insert(i + myShape.nPoint, 0, corrPos[i].Z);
	solveMatrix.setStructure(myShape.nPoint + nHandle, 1);
	mtMatrix.multi(solveMatrix, tmp);

	vecb.clear();
	tmp.changeToVec(vecb);

	ls.resetBX();
	ls.setVectorToB(vecb);
	ls.solve(true);

	for (i = 0; i < myShape.nPoint; ++i)
		myShape.points[i].Z = ls.x[i];


	myShape.update();

}

void Image::findHandlePoint()
{
	vector3df newPoint;
	int i, j, mj;
	int nMatch;
	double mind, tmp;

	handlePoint.clear();
	memset(isHandle, -1, sizeof(int)*myShape.nPoint);
	nHandle = 0;
	corrPos.clear();

	nMatch = matchSil3D.size();

	for (i = 0; i < nMatch; ++i)
	{
		if (matchSilIndex[i] == -1)
		{
			mind = InfDouble;
			for (j = 0; j < myShape.nPoint; ++j)
			{
				tmp = distance2(myShape.points[j], matchSil3D[i]);
				if (tmp < mind)
				{
					mind = tmp;
					mj = j;
				}
			}
			if (isHandle[mj] == -1)
			{
				isHandle[mj] = nHandle;
				handlePoint.push_back(mj);
				corrPos.push_back(reDefStroke3D[i]);
				++nHandle;
			}
		}
		else
		{
			if (isHandle[myShape.silEdges[matchSilIndex[i]].i] == -1)
			{
				newPoint = myShape.points[myShape.silEdges[matchSilIndex[i]].i] - matchSil3D[i] + reDefStroke3D[i];
				isHandle[myShape.silEdges[matchSilIndex[i]].i] = nHandle;
				handlePoint.push_back(myShape.silEdges[matchSilIndex[i]].i);
				corrPos.push_back(newPoint);
				++nHandle;
			}
			if (isHandle[myShape.silEdges[matchSilIndex[i]].j] == -1)
			{
				newPoint = myShape.points[myShape.silEdges[matchSilIndex[i]].j] - matchSil3D[i] + reDefStroke3D[i];
				isHandle[myShape.silEdges[matchSilIndex[i]].j] = nHandle;
				handlePoint.push_back(myShape.silEdges[matchSilIndex[i]].j);
				corrPos.push_back(newPoint);
				++nHandle;
			}
			if (myShape.isOnFace[matchSilIndex[i]])
			{
				if (isHandle[myShape.silEdges[matchSilIndex[i]].k] == -1)
				{
					newPoint = myShape.points[myShape.silEdges[matchSilIndex[i]].k] - matchSil3D[i] + reDefStroke3D[i];
					isHandle[myShape.silEdges[matchSilIndex[i]].k] = nHandle;
					handlePoint.push_back(myShape.silEdges[matchSilIndex[i]].k);
					corrPos.push_back(newPoint);
					++nHandle;
				}
			}
		}
	}

}

void Image::findAncher()
{
	int i;
	double r;
	priority_queue<QNode> que;
	QNode curNode, nextNode;
	double *minDis;
	set<int>::iterator it;
	bool *expanded, *inq;

	expanded = new bool[myShape.nPoint];
	memset(expanded, false, sizeof(bool)*myShape.nPoint);
	inq = new bool[myShape.nPoint];
	memset(inq, false, sizeof(bool)*myShape.nPoint);


	minDis = new double[myShape.nPoint];
	memset(minDis, 0, sizeof(double)*myShape.nPoint);

	for (i = 0; i < nHandle; ++i)
	{
		r = distance(myShape.points[handlePoint[i]], corrPos[i])*1.2;
		curNode.i = handlePoint[i];
		curNode.key = r;
		que.push(curNode);
		minDis[curNode.i] = r;
		inq[curNode.i] = true;
	}


	while (!que.empty())
	{
		curNode = que.top();
		que.pop();

		if (curNode.key < 0)
			continue;
		if (expanded[curNode.i])
			continue;

		for (it = myShape.adj[curNode.i].begin(); it != myShape.adj[curNode.i].end(); it++)
		{
			nextNode.i = *it;
			nextNode.key = curNode.key - distance(myShape.points[curNode.i], myShape.points[nextNode.i]);
			if (isHandle[nextNode.i] != -1 || expanded[nextNode.i])
				continue;
			if (minDis[nextNode.i] < nextNode.key)
			{
				minDis[nextNode.i] = nextNode.key;
				que.push(nextNode);
				inq[nextNode.i] = true;
			}
		}
		expanded[curNode.i] = true;

	}

	for (i = 0; i < myShape.nPoint; ++i)
		if (minDis[i] < ep && isHandle[i] == -1)
		{
		handlePoint.push_back(i);
		isHandle[i] = nHandle;
		corrPos.push_back(myShape.points[i]);
		++nHandle;
		}


	delete[]inq;
	delete[]expanded;
	delete[]minDis;

}

