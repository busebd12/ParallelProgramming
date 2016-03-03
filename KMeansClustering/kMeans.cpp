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

pthread_mutex_t secondKmeansMutex;

pthread_cond_t kmeansConditionVariable;

vector<vector<float>> buckets;

struct ThreadData
{
	vector<vector<float>> dataVector;

	vector<vector<float>> clusters;

	vector<float> clusterMeans;

	int sizeOfDataVector;
};

ThreadData threadData;

void printVector(vector<float> & V)
{
	for(const auto & element : V)
	{
		cout << element << " ";
	}

	cout << endl;
}

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

void calculateClusterMeans()
{
	vector<vector<float>> finalResult;

	finalResult.resize(numberOfClusters);

	for(auto & vec : finalResult)
	{
		vec.resize(numberOfColumns);
	}

	for(int x=0;x<buckets.size();++x)
	{
		for(int y=0;y<buckets.at(x).size();++y)
		{
			int temp=y%numberOfColumns;

			finalResult.at(x).at(temp)+=buckets.at(x).at(y);
		}
	}

	printClusters();

	for(int a=0;a<finalResult.size();++a)
	{
		for(int b=0;b<finalResult.at(a).size();++b)
		{
			finalResult.at(a).at(b)= finalResult.at(a).at(b) / (buckets.at(a).size()/numberOfColumns);

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

	cout << "Before the if-statement" << endl;

	cout << endl;

	if(threadRank==numberOfProcessors-1)
	{
		cout << "Inside if-statement" << endl;

		cout << endl;

		myEnd=numberOfDataPoints-numberOfColumns-1;
	}

	buckets.resize(numberOfClusters);

	float minimum=INT_MAX;

	float result;

	int spot=0;

	vector<float> elements;

	cout << "Before the nested for loops" << endl;

	cout << endl;

	for(int n=myStart;n<=myEnd-1;++n)
	{
		

		cout << "Before locking the mutex" << endl;

		cout << endl;

		for(int k=0;k<threadData.clusters.size();++k)
		{	
			cout << "Inside the second for loop" << endl;

			cout << endl;

			cout << "N: " << n << endl;

			cout << endl;

			cout << "thread " << threadRank << endl;

			cout << endl;


			result=calculateDistance(threadData.dataVector.at(n), threadData.clusters.at(k));

			cout << "After calling calculating distance function" << endl;

			cout << endl;

			if(minimum > result)
			{
				cout << "Inside the second if-statement" << endl;

				cout << endl;

				minimum=result;

				spot=k;
				
			}
			if(k==threadData.clusters.size()-1)
			{
				pthread_mutex_lock(&kmeansMutex);

				buckets[spot]=(threadData.dataVector.at(n));
				
				pthread_cond_broadcast(&kmeansConditionVariable);

				//buckets.at(spot).insert(buckets.at(spot).end(), threadData.dataVector.at(n).begin(), threadData.dataVector.at(n).end());
				pthread_mutex_unlock(&kmeansMutex);
			}
		}

		minimum=INT_MAX;

		cout << "Before unlocking the mutex" << endl;

		cout << endl;

		
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

	pthread_mutex_init(&secondKmeansMutex, NULL);

	pthread_cond_init(&kmeansConditionVariable, NULL);

	long thread;

	threadHandles=(pthread_t*)malloc(numberOfProcessors * sizeof(pthread_t));

	tic();

	for(thread=0;thread<numberOfProcessors;++thread)
	{
		pthread_create(&threadHandles[thread], NULL, kMeans, (void*)thread);
	}

	for(thread=0;thread<numberOfProcessors;++thread)
	{
		pthread_join(threadHandles[thread], NULL);
	}

	toc();

	cout << "Elapsed time: " << etime() << endl;

	cout << endl;

	calculateClusterMeans();
	
	pthread_mutex_destroy(&kmeansMutex);

	pthread_mutex_destroy(&secondKmeansMutex);

	pthread_cond_destroy(&kmeansConditionVariable);

	free(threadHandles);
}