#include <string.h>
#include <fstream>  
typedef struct profile{
	//int included[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int Quality;  // 1:Low,2:Standard,3:High
	bool Raft;
	bool Support;
	char LayerHeight[20];
	char Infill[20];
	char NumofShells[20];
	char Temperature[20];
	char ModelHeight[20];

	profile(){
		Quality = 1;
		Raft = true;
		Support = false;
		strcpy_s(LayerHeight, "0.1");
		strcpy_s(Infill, "10");
		strcpy_s(NumofShells, "2");
		strcpy_s(Temperature, "230");
		strcpy_s(ModelHeight, "20");
	}
}profile;

bool ctb(const char *a);
profile *read_settings(const char *filename);
