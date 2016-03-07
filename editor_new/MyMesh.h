#pragma once
#include "TriMesh.h"
#include <vector>

namespace trimesh
{

	class MyMesh :
		public TriMesh
	{
	public:
		MyMesh(const char *filename);
		std::vector<bool> selected,deletedV,deletedF;

	};

}