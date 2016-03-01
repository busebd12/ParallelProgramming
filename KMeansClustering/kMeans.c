#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./etime.h"
#include "./etime.c"

struct ThreadData
{
	double *firstColumn;

	double *secondColumn;

	double *firstCluster;

	double *secondCluster;

	int arraySize;
};

void printArray(double *Array, int ArrayLength)
{
	int index;

	for(index=0;index<ArrayLength;++index)
	{
		printf("%g ", Array[index]);
	}

	printf("\n");
}

void addElementToArray(double *Array, int ArraySize, double Element)
{
	int index;

	for(index=0;index<ArraySize;++index)
	{
		if(Array[index]==0.0)
		{
			printf("Spot %d has the value 0.0 \n", index);

			printf("\n");

			printf("Going to insert %g into spot %d \n \n", Element, index);

			break;
		}
	}
}

int removeElementAndReallocateArray(double *Array, int Index, int ArrayLength)
{
	/*
	printf("Array before deleting the element:\n");

	printArray(Array, ArrayLength);

	printf("\n");
	*/

	int start;

	/*Move elements backwards in array so as to fill the hole left by the removed element*/
	for(start=Index;start<ArrayLength-1;++start)
	{
		Array[start]=Array[start+1];
	}

	/*
	printf("Array after moving elements around:\n");

	printArray(Array, --ArrayLength);
	
	printf("\n");
	*/

	--ArrayLength;

	/*create temp array with size one less than original since we deleted an element*/
	double *temp=(double*)malloc(ArrayLength*sizeof(double));

	/*copy the values from the old array in the temp array*/
	for(start=0;start<ArrayLength;++start)
	{
		temp[start]=Array[start];
	}

	//printArray(temp, ArrayLength);

	/*set all elements of the original array to zero, effectively "deleting" them*/
	memset(&Array[0], 0, sizeof(Array));

	/*reallocate the size of the original array to one less than what we started with since we deleted an element*/
	Array=realloc(Array, (ArrayLength)*sizeof(Array));

	/*copy over elements from temp array to original array*/
	for(start=0;start<ArrayLength;++start)
	{
		Array[start]=temp[start];
	}

	/*
	printf("Array after removing element:\n");

	printArray(Array, ArrayLength);

	printf("\n");
	*/

	/*return the new array length*/
	return ArrayLength;
}

double calculateClusterMean(int ArraySize, double *Cluster)
{
	int count=0;

	int index;

	double sum=0.0;

	double mean;

	for(index=0;index<ArraySize;++index)
	{
		sum+=Cluster[index];

		count++;
	}

	mean=(double)(sum/count);

	return mean;
}

void* kMeans(void *parameters)
{
	struct ThreadData *threadData=parameters;	

	double firstClusterMean=calculateClusterMean(threadData->arraySize, threadData->firstCluster);

	printf("The intial mean of the first cluster is %g \n \n", firstClusterMean);

	double secondClusterMean=calculateClusterMean(threadData->arraySize, threadData->secondCluster);

	printf("The intial mean of the second cluster is %g \n", secondClusterMean);

	printf("\n");

	int index;

	double x2=firstClusterMean;

	double y2=0.0;

	double x3=secondClusterMean;

	double y3=0.0; 

	printf("Values in the kmeans function:\n");

	for(index=0;index<threadData->arraySize-(threadData->arraySize-2);++index)
	{
		printf("%g %g \n", threadData->firstColumn[index], threadData->secondColumn[index]);

		printf("\n");

		double firstEuclideanDistance;

		double secondEuclideanDistance;

		double x1=threadData->firstColumn[index];

		double y1=threadData->secondColumn[index];

		printf("Going to calculate the euclidean distance between (%g, %g) and (%g, %g) \n", x1, x2, y1, y2);

		printf("\n");

		/*calculates the euclidean distance between the coordinate and the mean of the first cluster using the standerd distance function*/
		firstEuclideanDistance=sqrt(pow((x1+x2), 2)+pow((y1+y2), 2));

		printf("Going to calculate the euclidean distance between (%g, %g) and (%g, %g) \n", x1, x3, y1, y3);

		printf("\n");

		/*calculates the euclidean distance between the coordinate and the mean of the second cluster using the standard distance function*/
		secondEuclideanDistance=sqrt(pow((x1+x3), 2)+pow((y1+y3),2));

		if(firstEuclideanDistance < secondEuclideanDistance)
		{
			printf("Going to insert %g into the first cluster \n", x1);

			printf("\n");

			//remove elements from their respective columns
			int size1=removeElementAndReallocateArray(threadData->firstColumn, index, threadData->arraySize);

			threadData->arraySize=size1;

			int size2=removeElementAndReallocateArray(threadData->secondColumn, index, threadData->arraySize);

			threadData->arraySize=size2;

			//add the coordinate to the first cluster
			addElementToArray(threadData->firstCluster, threadData->arraySize, x1);
		}
		else
		{
			printf("Going to insert %g into the second cluster \n", x1);

			printf("\n");

			//remove elements from their respective columns
			int size1=removeElementAndReallocateArray(threadData->firstColumn, index, threadData->arraySize);

			threadData->arraySize=size1;

			int size2=removeElementAndReallocateArray(threadData->secondColumn, index, threadData->arraySize);

			threadData->arraySize=size2;

			//add the coordinate to the second cluster
			addElementToArray(threadData->secondCluster, threadData->arraySize, x1);
		}	
	}

	printArray(threadData->firstColumn, threadData->arraySize);

	printf("\n");

	printArray(threadData->secondColumn, threadData->arraySize);
	
	return NULL;
}

int createInitialPartition(int ArraySize, double *FirstColumn, double *SecondColumn, double *FirstCluster, double *SecondCluster)
{
	int index;

	double currentMinimumOne=FirstColumn[0];

	double currentMinimumTwo=SecondColumn[0];

	double currentMaximumOne=FirstColumn[0];

	double currentMaximumTwo=SecondColumn[0];

	int maxOneIndex=0;

	int maxTwoIndex=0;

	int minOneIndex=0;

	int minTwoIndex=0;

	int localArraySize=0;

	int arraySizeCopy=ArraySize;

	//printf("\n Values in the createInitialPartition function: \n");

	for(index=0;index<ArraySize;++index)
	{
		//printf("%g %g \n", FirstColumn[index], SecondColumn[index]);

		if(FirstColumn[index] > currentMaximumOne)
		{
			currentMaximumOne=FirstColumn[index];

			maxOneIndex=index;
		}

		if(SecondColumn[index] > currentMaximumTwo)
		{
			currentMaximumTwo=SecondColumn[index];

			maxTwoIndex=index;
		}

		if(FirstColumn[index] < currentMinimumOne)
		{
			currentMinimumOne=FirstColumn[index];

			minOneIndex=index;
		}

		if(SecondColumn[index] < currentMinimumTwo)
		{
			currentMinimumTwo=SecondColumn[index];

			minTwoIndex=index;
		}
	}

	printf("The spot of the first maximum is: %d\n", maxOneIndex);

	printf("The spot of the second maximum is: %d \n", maxTwoIndex);

	printf("The spot of the first minimum is: %d \n", minOneIndex);

	printf("The spot of the second minimum is: %d \n", minTwoIndex);

	printf("\nThe smallest coordinate is: (%g, %g)\n", currentMinimumOne, currentMinimumTwo);

	printf("The largest coordinate is: (%g, %g)\n", currentMaximumOne, currentMaximumTwo);

	printf("\n");

	FirstCluster[0]=currentMinimumOne;

	FirstCluster[1]=currentMinimumTwo;

	SecondCluster[0]=currentMaximumOne;

	SecondCluster[1]=currentMaximumTwo;

	/*set the rest of the array spots to 0.0; this will come in handy later*/
	for(index=2;index<ArraySize;++index)
	{
		FirstCluster[index]=0.0;

		SecondCluster[index]=0.0;
	}

	localArraySize=removeElementAndReallocateArray(FirstColumn, minOneIndex, ArraySize);

	ArraySize=localArraySize;

	localArraySize=removeElementAndReallocateArray(FirstColumn, maxOneIndex, ArraySize);

	ArraySize=arraySizeCopy;

	localArraySize=removeElementAndReallocateArray(SecondColumn, maxTwoIndex, ArraySize);

	ArraySize=localArraySize;

	localArraySize=removeElementAndReallocateArray(SecondColumn, minTwoIndex, ArraySize);

	ArraySize=localArraySize;

	//printf("The new array size (from the createInitialPartition function) is: %d \n", ArraySize);

	return ArraySize;
}

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		fprintf(stderr, "Incorrect number of command line arguments. Program will exit gracefully now...");

		exit(0);
	}

	FILE *inputFile=fopen(argv[1], "r");

	if(inputFile==NULL)
	{
		fprintf(stderr, "Cannot open %s", argv[1]);

		exit(0);
	}

	struct ThreadData threadData;

	int arraySize=0;

	int numberOfColumns=0;

	int counter=0;

	double firstValue=0;

	double secondValue=0;

	fscanf(inputFile, "%d", &arraySize);

	fscanf(inputFile, "%d", &numberOfColumns);

	printf("The size for each array should be %d \n", arraySize);

	printf("The number of columns in the input file was %d \n", numberOfColumns);

	/*allocate memory for each array that will hold each column from the input file*/
	double *firstColumn=(double*)malloc(arraySize*sizeof(double));

	double *secondColumn=(double*)malloc(arraySize*sizeof(double));

	while(!feof(inputFile))
	{
		fscanf(inputFile, "%lg", &firstValue);

		fscanf(inputFile, "%lg", &secondValue);

		//printf("Reading value %g \n", firstValue);

		//printf("Reading value: %g \n", secondValue);

		if(firstValue==arraySize)
		{
			continue;
		}

		if(secondValue==numberOfColumns)
		{
			continue;
		}

		firstColumn[counter]=firstValue;

		secondColumn[counter]=secondValue;

		counter++;
	}

	printf("\n");

	int index;

	printf("Values: \n");

	for(index=0;index<arraySize;++index)
	{
		printf("%g %g \n", firstColumn[index], secondColumn[index]);
	}

	/*allocate memory for the two cluster arrays*/
	double *firstCluster=(double*)malloc(arraySize*sizeof(double));

	double *secondCluster=(double*)malloc(arraySize*sizeof(double));

	int newArraySize=createInitialPartition(arraySize, firstColumn, secondColumn, firstCluster, secondCluster);

	threadData.firstColumn=firstColumn;

	threadData.secondColumn=secondColumn;

	threadData.firstCluster=firstCluster;

	threadData.secondCluster=secondCluster;

	threadData.arraySize=newArraySize;

	printf("The inital partitions are:\n");

	int position;

	for(position=0;position<1;++position)
	{
		printf("The largest coordinate pair is: (%g, %g)\n", firstCluster[position], firstCluster[position]);

		printf("The smallest coordinate pair is: (%g, %g)\n", secondCluster[position], secondCluster[position]);
	}

	printf("\n");

	printf("\n");

	/*allocate space for thread array*/
	pthread_t* threadArray=malloc(arraySize*sizeof(pthread_t));

	pthread_t someThread;

	pthread_create(&someThread, NULL, kMeans, (void*)&threadData);

	/*
	use long just in case code is run on a 64-bit system
	long thread;

	for(thread=0;thead<arraySize;++thread)
	{
		pthread_create(&threadArray[thread], NULL, kMeans, )
	}
	*/

	pthread_join(someThread, NULL);

	free(firstCluster);

	free(secondCluster);

	free(firstColumn);

	free(secondColumn);

	fclose(inputFile);
}