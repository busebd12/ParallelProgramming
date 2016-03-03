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
using namespace std;

int numberOfColumns;

int numberOfDataPoints;

int numberOfClusters;

int numberOfProcessors;

struct ThreadData
{
	vector<vector<float>> dataVector;

	vector<vector<float>> clusters;

	vector<float> clusterMeans;

	int sizeOfDataVector;
};

void printDataVector(const ThreadData & T)
{
	for(const auto & vec : T.dataVector)
	{
		for(const auto & element : vec)
		{
			cout << element << " ";
		}

		cout << endl;
	}
}

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

	cout << endl;
}

void addCoordinateToCluster(ThreadData & T, int PositionOfCluster, vector<float> & SamplePoint)
{
	T.clusters.at(PositionOfCluster).insert(begin(T.clusters.at(PositionOfCluster)), SamplePoint.begin(), SamplePoint.end());
}

float calculateClusterMeanHelper(vector<float> & Cluster)
{
	int size=Cluster.size();

	float sum=accumulate(Cluster.begin(), Cluster.end(), 0.0);

	float mean=(float)(sum/size);

	return mean;
}

void calculateClusterMeans(vector<vector<float>> Buckets, ThreadData & T)
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

	printClusters(T);

	for(int a=0;a<finalResult.size();++a)
	{
		for(int b=0;b<finalResult.at(a).size();++b)
		{
			finalResult.at(a).at(b)= finalResult.at(a).at(b) / (Buckets.at(a).size()/numberOfColumns);

			T.clusters.at(a)=finalResult.at(a);
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

void addSampleToCluster(vector<float> & Cluster, float Sample)
{
	Cluster.push_back(Sample);
}

void createClusters(ThreadData & T)
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
		for(int count=0;count<T.dataVector.size();++count)
		{
			if(randomPositions.at(index)==count)
			{
				T.clusters.push_back(T.dataVector.at(count));

				T.dataVector.erase(begin(T.dataVector)+count);

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

	printClusters(T);
}

float calculateDistance(vector<float> & N, vector<float> & K)
{
	/*
	cout << "Hello, from calculateDistance" << endl;

	cout << endl;
	*/

	float sum {};

	for(int location=0;location<N.size();++location)
	{
		sum+=pow((K.at(location)-N.at(location)), 2);
	}

	sum=sqrt(sum);

	/*
	cout << "Going to return the sum of: " << sum << endl;

	cout << endl;
	*/	

	return sum;
}

void* kMeans(void *parameters)
{
	ThreadData *threadData=(ThreadData*)parameters;

	vector<vector<float>> buckets;

	buckets.resize(numberOfClusters);

	float minimum=INT_MAX;

	float result;

	int spot=0;

	vector<float> elements;

	for(int n=0;n<numberOfDataPoints-numberOfClusters;++n)
	{
		for(int k=0;k<numberOfClusters;++k)
		{	
			result=calculateDistance(threadData->dataVector.at(n), threadData->clusters.at(k));

			if(minimum > result)
			{
				minimum=result;

				spot=k;
				
				elements=threadData->dataVector.at(n);
			}
		}

		for(int b=0;b<numberOfColumns;++b)
		{
			buckets.at(spot).push_back(elements.at(b));
		}	

		minimum=INT_MAX;
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
	calculateClusterMeans(buckets, *threadData);
	
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

		T.dataVector.push_back(temp);
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

	printDataVector(threadData);

	createClusters(threadData);

	printClusters(threadData);

	pthread_t someThread;

	pthread_create(&someThread, NULL, kMeans, (void*)&threadData);

	pthread_join(someThread, NULL);
}