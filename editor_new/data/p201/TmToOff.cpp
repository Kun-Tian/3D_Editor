#include <fstream>
#include <iostream>
#include <stdio.h>
using namespace std;


int main()
{
	char filename[200] = "p201_4.tm";
	char outfilename[200];
	int n, m, tmpi, i;
	char format[20];
	double tmp;
	
	strcpy(outfilename, filename);
	strcat(outfilename, ".off");
	
	ifstream inFile(filename);
	ofstream outFile(outfilename);

	if (!inFile)
	{
		cerr<<"File '"<<filename<<"' failed to load!"<<endl;
		exit(0);
	}
	
	outFile<<"OFF\n";
	inFile>>n;
	inFile>>m;
	outFile<<n<<" "<<m<<endl;
	outFile<<"0\n";	
	

	for (i = 0; i < n; i++)
	{
		inFile>>tmp;
		outFile<<tmp<<" ";
		inFile>>tmp;
		outFile<<tmp<<" ";
		inFile>>tmp;		
		outFile<<tmp<<endl;
	}
	for (i = 0; i < m; i++)
	{
		outFile<<"3 ";
		inFile>>tmpi;
		outFile<<tmpi<<" ";
		inFile>>tmpi;
		outFile<<tmpi<<" ";
		inFile>>tmpi;
		outFile<<tmpi<<endl;
	}

	inFile.clear();
	outFile.clear();
	
	return 0;
}
