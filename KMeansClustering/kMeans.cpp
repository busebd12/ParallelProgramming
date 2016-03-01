#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include <pthread.h>
using namespace std;

struct ThreadData
{
	vector<vector<int>> data;
};

int main(int argc, char *argv[])
{
	if(argc!=4)
	{
		cerr << "Incorrect number of command line arguments. The program will exit gracefully now..." << endl;

		exit(0);
	}

	int numberOfClusters=atoi(argv[1]);

	int numberOfProcessors=atoi(argv[2]);

	ifstream infile;

	infile.open(argv[3]);

	if(!(infile.open()))
	{
		cout << "Can't open " + argv[1] << endl;

		exit(0);
	}

}