#include "mesh3d.h"

//constructor
mesh3d::mesh3d()
{
	numOfObjects = 0;

	mObjects.clear();

	isInit = false;
	isHalfedge = false;
}
mesh3d::mesh3d(const mesh3d& mesh)
{
	//////////////////to be edited
}
mesh3d::~mesh3d()
{
	//////////////////to be edited
}

//function
void mesh3d::loadFromOFF(const string &filename) //从OFF文件中读入模型，默认为单个obj
{
	ifstream inFile(filename.c_str());

	if (!inFile)
	{
		cerr << "File '" << filename.c_str() << "' failed to load!" << endl;
		exit(0);
	}

	mObject obj;
	mVert curVert;
	mFacet curFacet;
	int tmpi;
	int i;
	char format[256];

	numOfObjects = 1;
	mObjects.clear();

	obj.selfIndex = 0;
	//	obj.meshHE = NULL;
	obj.isHalfedge = false;

	inFile >> format;
	inFile >> obj.numOfVerts;
	inFile >> obj.numOfFacets;
	inFile >> tmpi;

	for (i = 0; i < obj.numOfVerts; i++)
	{
		curVert.selfIndex = i;
		inFile >> curVert.X;
		inFile >> curVert.Y;
		inFile >> curVert.Z;

		obj.mVerts.push_back(curVert);
	}
	for (i = 0; i < obj.numOfFacets; i++)
	{
		curFacet.selfIndex = i;
		inFile >> tmpi;
		inFile >> curFacet.indexOfPointA;
		inFile >> curFacet.indexOfPointB;
		inFile >> curFacet.indexOfPointC;

		obj.mFacets.push_back(curFacet);
	}

	mObjects.push_back(obj);				///////////////////////
	numOfObjects = 1;						///////////////////////

	inFile.clear();
	isInit = true;
}
void mesh3d::loadFrom3DS(const string &filename)
{
	ifstream inFile(filename.c_str());

	if (!inFile)
	{
		cerr << "File '" << filename.c_str() << "' failed to load!" << endl;
		exit(0);
	}

	//////////////////////to be edited
}
void mesh3d::loadFromOBJ(const string &filename)
{
	ifstream inFile(filename.c_str());

	if (!inFile)
	{
		cerr << "File '" << filename.c_str() << "' failed to load!" << endl;
		exit(0);
	}

	////////////////////////to be edited
}
/*void mesh3d::convertToHEMesh(int indexOfObj)
{
mObject &obj = mObjects[indexOfObj];
obj->meshHE = new HEMesh();
obj->meshHE->initFromMesh(obj->mVerts, obj->mFacets);
}*/

/*bool mesh3d::isInit()
{
return isInit;
}
bool mesh3d::isHalfedge()
{
return isHalfedge;
}*/



