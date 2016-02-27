#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "./etime.h"
#include "./etime.c"

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

	free(firstColumn);

	free(secondColumn);

	fclose(inputFile);
}