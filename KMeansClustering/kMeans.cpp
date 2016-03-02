#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <pthread.h>
using namespace std;

int numberOfColumns;

int numberOfDataPoints;

int numberOfClusters;

int numberOfProcessors;

struct ThreadData
{
	vector<vector<double>> dataVector;

	vector<vector<double>> clusters;

	int sizeOfDataVector;
};

void printClusters(const ThreadData & T)
{
	cout << "Clusters:" << endl;

	for(const auto & cluster : T.clusters)
	{
		for(const auto & sample : cluster)
		{
			cout << sample << " ";
		}

		cout << endl;
	}
}

double getRandomSample(ThreadData & T, int VectorPosition, int SamplePosition)
{
	double sample=T.dataVector.at(VectorPosition).at(SamplePosition);

	return sample;
}

void createClusters(ThreadData & T)
{
	vector<double> dataHolder;

	random_device randomDevice;

	mt19937 generator(randomDevice());

	for(int count=0;count<numberOfClusters;++count)
	{
		uniform_int_distribution<> distribution(1, numberOfDataPoints);

		int positionOfRandomSample=distribution(generator);

		double randomSample=getRandomSample(T, count, positionOfRandomSample);

		dataHolder.push_back(randomSample);
	}

	int x=0;

	while(x < numberOfClusters)
	{
		vector<double> temp;

		for(int index=x;index<dataHolder.size();index+=numberOfClusters)
		{
			temp.push_back(dataHolder.at(index));
		}

		T.clusters.push_back(temp);

		x++;
	}
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

	cout << endl;

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

	numberOfClusters=atoi(argv[1]);

	numberOfProcessors=atoi(argv[2]);

	ifstream infile;

	infile.open(argv[3]);

	ThreadData threadData;

	readInData(infile, threadData);

	createClusters(threadData);

	printClusters(threadData);

	/*
	pthread_t someThread;

	pthread_create(&someThread, NULL, kMeans, (void*)&threadData);

	pthread_join(someThread, NULL);
	*/
}