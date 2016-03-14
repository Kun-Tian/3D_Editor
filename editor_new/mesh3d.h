#ifndef MESH3D_H
#define MESH3D_H

#include <iostream>
#include <fstream>
#include "CommonStructure.h"
//#include "meshOfHE.h"

using namespace std;

class mesh3d //ֻ�������������
{
public:
	//constructor!
	mesh3d();
	mesh3d(const mesh3d& mesh);
	~mesh3d();

	//operator!
	//function!
	void loadFrom3DS(const string &filename);
	void loadFromOFF(const string &filename);
	void loadFromOBJ(const string &filename);
//	void convertToHEMesh(int indexOfObj);

//	bool isInit();
//	bool isHalfedge();
	bool isInit; //�Ƿ��ѳɹ���ʼ��
	bool isHalfedge;

	virtual void draw() = 0;

protected:
	vector<mObject> mObjects;
	
	int numOfObjects;
	

};


#endif