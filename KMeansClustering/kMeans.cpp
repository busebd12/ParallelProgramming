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

int bucketPlace;

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
			cout << "buckets.at(x).at(y):" << buckets.at(x).at(y) << endl;

			cout << endl;

			int temp=y%numberOfColumns;

			cout << "temp: " << temp << endl;

			//cout << endl;

			finalResult.at(x).at(temp)+=buckets.at(x).at(y);
		}
	}

	printClusters();

	for(int a=0;a<threadData.clusters.size();++a)
	{
		for(int b=0;b<threadData.clusters.at(a).size();++b)
		{
			finalResult.at(a).at(b)= finalResult.at(a).at(b) / (buckets.at(a).size()/numberOfColumns);

			if(isnan(finalResult.at(a).at(b)))
			{
				cout << "a: " << a <<  " b: " << b << endl;

				cout << "finalResult.at(a).at(b) is nan and threadData.clusters.at(a).at(b) is: " << threadData.clusters.at(a).at(b) << endl;

				cout << endl;

				finalResult.at(a).at(b)=threadData.clusters.at(a).at(b);
			}


			threadData.clusters.at(a).at(b)=finalResult.at(a).at(b);
		}
	}

	cout << "Final result vector:" << endl;

	printClusters();
}

void createClusters()
{
	cout << "In create clusters function" << endl;

	cout << endl;

	random_device randomDevice;

	mt19937 generator(randomDevice());

	uniform_int_distribution<int> distribution(1, ((numberOfDataPoints/numberOfColumns)-1));

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
		//cout << "Calculating the distance between " << K.at(location) << " and " << N.at(location) << endl;

		//cout << endl;

		sum+=pow((K.at(location)-N.at(location)), 2);
	}

	sum=sqrt(sum);

	cout << "Return sum of: " << sum << endl;

	return sum;
}

void* kMeans(void *parameters)
{
	long threadRank=(long)parameters;

	cout<<"KMeans"<<endl;

	int chunkSize=(numberOfDataPoints-numberOfClusters)/numberOfProcessors;

	int myStart=threadRank*chunkSize;

	int myEnd=threadRank*chunkSize+chunkSize;

	//cout << "Before the if-statement" << endl;

	cout << endl;

	if(threadRank==numberOfProcessors-1)
	{
		//cout << "Inside if-statement" << endl;

		//cout<<threadRank<<" threadRank inside statement"<<endl;

		cout << endl;

		myEnd=numberOfDataPoints-numberOfClusters;
	}

	buckets.resize(numberOfClusters);

	float minimum=INT_MAX;

	float result;

	int spot=0;

	vector<float> elements;

	//cout << "Before the nested for loops" << endl;

	//pthread_mutex_lock(&kmeansMutex);

	//cout<<myStart<<" :myStart, "<<myEnd<<" myEnd"<<endl;


	for(int n=myStart;n<myEnd;++n)
	{
		//cout << "Before locking the mutex" << endl;

		//cout << endl;
		pthread_mutex_lock(&kmeansMutex);
		for(int k=0;k<threadData.clusters.size();++k)
		{	
			/*
			cout << "Inside the second for loop" << endl;

			cout << endl;

			cout << "N: " << n << endl;

			cout << endl;

			cout << "thread " << threadRank << endl;

			cout << endl;
			*/

			result=calculateDistance(threadData.dataVector.at(n), threadData.clusters.at(k));

			/*
			cout << "After calling calculating distance function" << endl;

			cout << endl;
			*/

			if(minimum > result)
			{
				//cout << "Inside the second if-statement" << endl;

				//cout << endl;

				minimum=result;

				spot=k;
				
			}

			if(k==threadData.clusters.size()-1)
			{

				cout<<"Adds to Bucket"<<endl;

				for(int index=0;index<numberOfColumns;++index)
				{
					buckets.at(spot).push_back(threadData.dataVector.at(n).at(index));
				}
			}
		}

		minimum=INT_MAX;
		pthread_mutex_unlock(&kmeansMutex);

		//cout << "Before unlocking the mutex" << endl;

		//cout << endl;
	}

	//pthread_cond_broadcast(&kmeansConditionVariable);

	//pthread_mutex_unlock(&kmeansMutex);

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
	int place = 0;
	for(int index=0;index<dataHolder.size()-numberOfColumns;index++)
	{
		vector<float> temp;
		place=0;
		while(place<numberOfColumns && index<dataHolder.size())
		{
			temp.push_back(dataHolder.at(index));
			index++;
			place++;
		}

		/*
		cout << "Adding " << dataHolder.at(index) << " to the vector" << endl;

		cout << endl;
		*/
		cout<<index<<endl;
		cout<<temp.size()<<endl;;
		threadData.dataVector.push_back(temp);
	}
	cout<<"getsout"<<endl;
}

int main(int argc, char *argv[])
{
	bucketPlace =0;
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

	//printDataVector();

	createClusters();

	/*Initialize mutex and condition variable objects*/
	pthread_mutex_init(&kmeansMutex, NULL);

	long thread;

	threadHandles=new pthread_t[numberOfProcessors];
	//threadHandles = malloc(numberOfProcessors*sizeof(pthread_t));
	vector<vector<float>> sampleCluster;
	sampleCluster.resize(numberOfClusters);

	for(auto & vec : sampleCluster)
	{
		vec.resize(numberOfColumns);
	}

	int check =0;
	int base=0;
	tic();

	for(int i = 0; i<100; i++)
	{

		for(int a = 0; a<threadData.clusters.size(); a++)
		{
			for(int b = 0; b<threadData.clusters.at(a).size(); b++)
			{
				sampleCluster.at(a).at(b) = threadData.clusters.at(a).at(b);
			}
		}

	for(thread=0;thread<numberOfProcessors;++thread)
	{
		pthread_create(&threadHandles[thread], NULL, kMeans, (void*)thread);
	}

	for(thread=0;thread<numberOfProcessors;++thread)
	{
		pthread_join(threadHandles[thread], NULL);
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

	calculateClusterMeans();
	
	pthread_mutex_destroy(&kmeansMutex);

	for(int a = 0; a<threadData.clusters.size(); a++)
	{
		for(int b = 0; b<threadData.clusters.at(a).size(); b++)
		{
			base++;
			if(sampleCluster.at(a).at(b) == threadData.clusters.at(a).at(b))
				check++;
		}
	}
	if(check==base)
	{
		break;
	}
	cout<<i<<" The End"<<endl;
	}

	toc();

	cout << "Elapsed time: " << etime() << endl;

	cout << endl;

	delete [] threadHandles;
}