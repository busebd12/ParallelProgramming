#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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

void* kMeans(void *parameters)
{
	struct ThreadData *threadData=parameters;	

	int index;

	printf("Values inside the kMeans function:\n");

	for(index=0;index<threadData->arraySize;++index)
	{
		printf("%g %g \n", threadData->firstColumn[index], threadData->secondColumn[index]);
	}

	return NULL;
}

void createInitialPartition(int ArraySize, double *FirstColumn, double *SecondColumn, double *FirstCluster, double *SecondCluster)
{
	int index;

	double currentMinimumOne=FirstColumn[0];

	double currentMinimumTwo=SecondColumn[0];

	double currentMaximumOne=FirstColumn[0];

	double currentMaximumTwo=SecondColumn[0];

	//printf("\n Values in the createInitialPartition function: \n");

	for(index=0;index<ArraySize;++index)
	{
		//printf("%g %g \n", FirstColumn[index], SecondColumn[index]);

		if(FirstColumn[index] > currentMaximumOne)
		{
			currentMaximumOne=FirstColumn[index];
		}

		if(SecondColumn[index] > currentMaximumTwo)
		{
			currentMaximumTwo=SecondColumn[index];
		}

		if(FirstColumn[index] < currentMinimumOne)
		{
			currentMinimumOne=FirstColumn[index];
		}

		if(SecondColumn[index] < currentMinimumTwo)
		{
			currentMinimumTwo=SecondColumn[index];
		}
	}

	printf("\nThe smallest coordinate is: (%g, %g)\n", currentMinimumOne, currentMinimumTwo);

	printf("The largest coordinate is: (%g, %g)\n", currentMaximumOne, currentMaximumTwo);

	printf("\n");

	FirstCluster[0]=currentMinimumOne;

	FirstCluster[1]=currentMinimumTwo;

	SecondCluster[0]=currentMaximumOne;

	SecondCluster[1]=currentMaximumTwo;
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

	createInitialPartition(arraySize, firstColumn, secondColumn, firstCluster, secondCluster);

	threadData.firstColumn=firstColumn;

	threadData.secondColumn=secondColumn;

	threadData.firstCluster=firstCluster;

	threadData.secondCluster=secondCluster;

	threadData.arraySize=arraySize;

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