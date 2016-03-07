#include "MyMesh.h"

namespace trimesh
{
	MyMesh::MyMesh(const char *filename)
	{
		TriMesh::read_helper(filename, this);
		need_faces();
		selected.resize(vertices.size(), false);
		deletedV.resize(vertices.size(), false);
		deletedF.resize(faces.size(), false);
	}
}