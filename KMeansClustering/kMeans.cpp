#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <pthread.h>
using namespace std;

struct ThreadData
{
	vector<vector<double>> dataVector;

	vector<vector<double>> clusters;

	int sizeOfDataVector;
};

void createClusters(ThreadData & T, int NumberOfClusters)
{
	
}

void* kMeans(void *parameters)
{
	cout << "Hello, from the kMeans function" << endl;

	cout << endl;

	ThreadData *threadData=(ThreadData*)parameters;

	cout << "The size of the data vector in the kMeans function is: " << threadData->sizeOfDataVector << endl;

	cout << endl;	
	
	return NULL;
}

void readInData(ifstream & File, ThreadData & T)
{
	if(!(File.is_open()))
	{
		cout << "Can't open file" << endl;

		exit(0);
	}

	int count=0;

	int input;

	int numberOfColumns;

	int numberOfDataPoints;

	while(count < 2)
	{
		File >> input;

		if(count==0)
		{
			numberOfDataPoints=input;
		}
		else if(count==1)
		{
			numberOfColumns=input;
		}

		count++;
	}

	T.sizeOfDataVector=(numberOfDataPoints*numberOfColumns);

	cout << "The number of data points is " << numberOfDataPoints << endl;

	cout << endl;

	cout << "The number of columns is " << numberOfColumns << endl;

	string s1, s2;

	string line;

	int counter=0;

	vector<double> dataHolder;

	while(getline(File, line))
	{
		/*
		cout << "counter " << counter<< endl;

		cout << endl;
		*/

		double input1;

		double input2;

		stringstream ss(line);

		ss >> input1;

		/*
		cout << "input1: " << input1 << endl;

		cout << endl;
		*/

		if(counter!=0)
		{
			/*
			cout << "Adding " << input1 << " to dataHolder" << endl;

			cout << endl;
			*/

			dataHolder.push_back(input1);			
		}

		ss >> input2;

		/*
		cout << "input2: " << input2 << endl;

		cout << endl;
		*/

		if(counter!=0)
		{
			/*
			cout << "Adding " << input2 << " to dataHolder" << endl;

			cout << endl;
			*/

			dataHolder.push_back(input2);
		}

		counter++;
	}

	int i=0;

	while(i < numberOfColumns)
	{
		vector<double> temp;

		for(int index=0;index<dataHolder.size();index+=numberOfColumns)
		{
			/*
			cout << "Adding " << dataHolder.at(index) << " to the vector" << endl;

			cout << endl;
			*/

			temp.push_back(dataHolder.at(index));
		}

		T.dataVector.push_back(temp);

		i++;
	}
}

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

	ThreadData threadData;

	readInData(infile, threadData);

	cout << "The size of the vectors of vectors is: " << threadData.sizeOfDataVector << endl;

	cout << endl;

	pthread_t someThread;

	cout << "After declaring our thread" << endl;

	cout << endl;

	cout << "Before creating the thread" << endl;

	cout << endl;

	pthread_create(&someThread, NULL, kMeans, (void*)&threadData);

	cout << "After the call to the pthread_create function" << endl;

	cout << endl;

	pthread_join(someThread, NULL);
}