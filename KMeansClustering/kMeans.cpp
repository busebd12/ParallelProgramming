#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <fstream>
#include <string>
#include <complex>
#include <sstream>
#include <random>
#include <cmath>
#include <pthread.h>
#include <climits>
#include "etime.h"
using namespace std;

int numberOfColumns;

int numberOfDataPoints;

int numberOfClusters;

int numberOfProcessors;

pthread_mutex_t kmeansMutex;

pthread_cond_t kmeansConditionVariable;



struct ThreadData
{
	vector<vector<float>> dataVector;

	vector<vector<float>> clusters;

	vector<float> clusterMeans;

	int sizeOfDataVector;
};

ThreadData threadData;

void printDataVector()
{
	for(const auto & vec : threadData.dataVector)
	{
		for(const auto & element : vec)
		{
			cout << element << " ";
		}

		cout << endl;
	}
}

void printClusters()
{
	cout << "Clusters:" << endl;

	for(const auto & cluster : threadData.clusters)
	{
		for(const auto & sample : cluster)
		{
			cout << sample << " ";
		}

		cout << endl;
	}

	cout << endl;
}

void calculateClusterMeans(vector<vector<float>> Buckets)
{
	vector<vector<float>> finalResult;

	finalResult.resize(numberOfClusters);

	for(auto & vec : finalResult)
	{
		vec.resize(numberOfColumns);
	}

	for(int x=0;x<Buckets.size();++x)
	{
		for(int y=0;y<Buckets.at(x).size();++y)
		{
			int temp=y%numberOfColumns;

			finalResult.at(x).at(temp)+=Buckets.at(x).at(y);
		}
	}

	printClusters();

	for(int a=0;a<finalResult.size();++a)
	{
		for(int b=0;b<finalResult.at(a).size();++b)
		{
			finalResult.at(a).at(b)= finalResult.at(a).at(b) / (Buckets.at(a).size()/numberOfColumns);

			threadData.clusters.at(a)=finalResult.at(a);
		}
	}

	cout << "Final result vector:" << endl;

	for(const auto & bucket : finalResult)
	{
		for(const auto & element : bucket)
		{
			cout << element << " ";
		}

		cout << endl;
	}
}

void createClusters()
{
	cout << "In create clusters function" << endl;

	cout << endl;

	random_device randomDevice;

	mt19937 generator(randomDevice());

	uniform_int_distribution<int> distribution(1, (numberOfDataPoints-1));

	vector<int> randomPositions;

	int randomPosition;

	bool flag=true;

	int check=0;

	for(int count=0;count<numberOfClusters;++count)
	{
		cout << "randomPositions size: " << randomPositions.size() << endl;

		cout << endl;

		randomPosition=distribution(generator);

		cout << "Random number: " << randomPosition << endl;

		randomPositions.push_back(randomPosition);	
	}

	for(int index=0;index<randomPositions.size();++index)
	{
		for(int count=0;count<threadData.dataVector.size();++count)
		{
			if(randomPositions.at(index)==count)
			{
				threadData.clusters.push_back(threadData.dataVector.at(count));

				threadData.dataVector.erase(begin(threadData.dataVector)+count);

				for(int i=0;i<randomPositions.size();++i)
				{
					if(randomPositions.at(index) < randomPositions.at(i) && index != i)
					{
						randomPositions.at(i)-=1;	
					}	
				}
			}
		}
	}

	printClusters();
}

float calculateDistance(vector<float> & N, vector<float> & K)
{
	float sum {};

	for(int location=0;location<N.size();++location)
	{
		sum+=pow((K.at(location)-N.at(location)), 2);
	}

	sum=sqrt(sum);

	return sum;
}

void* kMeans(void *parameters)
{
	long threadRank=(long)parameters;

	int chunkSize=(numberOfProcessors-numberOfColumns)/numberOfProcessors;

	int myStart=threadRank*chunkSize+1;

	int myEnd=threadRank*chunkSize+chunkSize;

	if(threadRank==numberOfProcessors-1)
	{
		myEnd=numberOfDataPoints-numberOfColumns;
	}

	vector<vector<float>> buckets;

	buckets.resize(numberOfClusters);

	float minimum=INT_MAX;

	float result;

	int spot=0;

	vector<float> elements;

	for(int n=myStart;n<=myEnd;++n)
	{
		pthread_mutex_lock(&kmeansMutex);

		for(int k=n+1;k<n;++k)
		{	
			result=calculateDistance(threadData.dataVector.at(n), threadData.clusters.at(k));

			if(minimum > result)
			{
				minimum=result;

				spot=k;
				
				elements=threadData.dataVector.at(n);
			}
		}

		for(int b=0;b<numberOfColumns;++b)
		{
			buckets.at(spot).push_back(elements.at(b));
		}	

		minimum=INT_MAX;

		pthread_mutex_unlock(&kmeansMutex);
	}

	cout << "Bucket vector after:" << endl;

	for(const auto & bucket : buckets)
	{
		for(const auto & element : bucket)
		{
			cout << element << " ";
		}

		cout << endl;
	}

	calculateClusterMeans(buckets);
	
	return NULL;
}

void readInData(ifstream & File)
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

	threadData.sizeOfDataVector=(numberOfDataPoints*numberOfColumns);

	cout << "The number of data points is " << numberOfDataPoints << endl;

	cout << endl;

	cout << "The number of columns is " << numberOfColumns << endl;

	cout << endl;

	string s1, s2;

	string line;

	int counter=0;

	vector<float> dataHolder;

	while(getline(File, line))
	{
		/*
		cout << "counter " << counter<< endl;

		cout << endl;
		*/

		float input1;

		float input2;

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

	for(int index=0;index<dataHolder.size();index+=numberOfColumns)
	{
		vector<float> temp;

		for(int position=index;position<(numberOfColumns+index);++position)
		{
			temp.push_back(dataHolder.at(index));
		}

		/*
		cout << "Adding " << dataHolder.at(index) << " to the vector" << endl;

		cout << endl;
		*/

		threadData.dataVector.push_back(temp);
	}
}

int main(int argc, char *argv[])
{
	if(argc!=4)
	{
		cerr << "Incorrect number of command line arguments. The program will exit gracefully, now..." << endl;

		exit(0);
	}

	pthread_t* threadHandles;

	numberOfClusters=atoi(argv[1]);

	numberOfProcessors=strtol(argv[2],NULL,10);

	ifstream infile;

	infile.open(argv[3]);

	readInData(infile);

	printDataVector();

	createClusters();

	printClusters();

	/*Initialize mutex and condition variable objects*/
	pthread_mutex_init(&kmeansMutex, NULL);

	long thread;

	threadHandles = malloc(numberOfProcessors * sizeof(pthread_t));

	for(thread=0;thread<numberOfProcessors;++thread)
	{
		pthread_create(&threadHandles[thread], NULL, kMeans, (void*)thread);
	}

	for(thread=0;thread<numberOfProcessors;++thread)
	{
		pthread_join(threadHandles[thread], NULL);
	}

	pthread_mutex_destroy(&kmeansMutex);
	free(threadHandles);

}