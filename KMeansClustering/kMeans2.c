#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include "etime.h"

long threadCount;

int K;

int Samples;

int Dimensions;

int flagOne=1;

int flagTwo=1;

pthread_mutex_t kmeansMutex;

pthread_barrier_t kmeansBarrier;

int *sampleClusterMap; //Creating sampleClusterMap to map Samples to corresponding cluster

int *priorSampleClusterMap; //Secondary sampleClusterMap to check when to threadEnd 100x loop

int *clusterElements; //Number of Samples in each cluster

int *randomCheck; //Making sure no duplicate initial cluster coordinates are generated

double **data; //Creating data[Samples][Dimensions] array to store Samples its coordinates

double **clusterCoordinateInfo; //Creating clustebarrierResultoordinateInfo to store coordinates of each cluster

void *kMeans (void* rank);

int main(int argc, char* argv[]) 
{


	FILE *infile, *outfile;

	/*read in command line information*/
	if(argc!=5)
	{
		printf("incorrect parameters.\n");

		exit(0);
	} 
	else
	{
		K=strtol(argv[1], NULL, 10);

		threadCount=strtol(argv[2], NULL, 10);

		infile=fopen(argv[3], "r");

		outfile=fopen(argv[4], "w+");
	}

   	/*Store sample and dimension info*/
	fscanf(infile, "%i", &Samples);

	fscanf(infile, "%i", &Dimensions);

	//printf("Clusters: %i, Processors: %ld\n", K, threadCount);
	//printf("Samples: %i, Dimensions: %i\n", Samples, Dimensions);

	/*counter variables for all for loops*/
   	int index;

   	int position;

   	//initializing all globals
   	sampleClusterMap=(int*)malloc(Samples*sizeof(int));

   	priorSampleClusterMap=(int*)malloc(Samples*sizeof(int));

   	clusterElements=(int *)malloc(K*sizeof(int));

   	randomCheck=(int*)malloc(Samples*sizeof(int));

   	data=(double**)malloc(Samples*sizeof(double*));

   	clusterCoordinateInfo=(double **)malloc(K*sizeof(double*));

	//Continue allocating 2nd Dimensions to data[Samples][Dimensions]
    for (index=0; index<Samples;++index)
    {
         data[index]=(double*)malloc(Dimensions*sizeof(double));

         priorSampleClusterMap[index] = 0;
    }

    //Reading in coordinates to array of arbitrary dimension
    double buffer;
    for(index=0;index<Samples;++index)
    {
	    	randomCheck[index] = 0;

	    	for (position=0; position<Dimensions;++position) 
	    	{
	    		fscanf(infile, "%lf", &buffer);

	    		data[index][position]=buffer;
	    	}
    }

    //Continue allocating 2nd dimension to clustebarrierResultoordinateInfo[K][Dimensions]
    for (index=0;index<K;++index)
    {
    		clusterCoordinateInfo[index]=(double*)malloc(Dimensions*sizeof(double));
    }


    //2. Seting initial cluster coordinates as coordinates
    srand(time(NULL));
    for (index=0;index<K;++index)
    {
	    	int randData = rand() % Samples;

	    	while(randomCheck[randData] == 1)
	    	{
	    		randData = rand() % Samples;
	    	}

    		randomCheck[randData] = 1;

		for (position=0;position<Dimensions;position++)
		{
			clusterCoordinateInfo[index][position] = data[randData][position];
			//clustebarrierResultoordinateInfo[i][position] = data[i][position]; //Populate initial cluster with first K Samples (ie. cluster 1 = sample 1)
			//printf("Cluster %i:%i = %lf ", i, position, clustebarrierResultoordinateInfo[i][position]);
		}
		//printf("\n");
	}
	//printf("\n");

	//PThreads setup
	long thread;  /* Use long in case of a 64-bit system */

   	pthread_t* thread_handles;

   	thread_handles = (pthread_t*) malloc (threadCount*sizeof(pthread_t)); 

   	pthread_mutex_init(&kmeansMutex, NULL); //For kmeansMutex

   	pthread_barrier_init(&kmeansBarrier, NULL, threadCount); //For kmeansBarrier synchronization

	tic();
	
	//==============================================================
	
	for (thread = 0; thread < threadCount; thread++)  
		pthread_create(&thread_handles[thread], NULL, kMeans, (void*)thread);  


	for (thread = 0; thread < threadCount; thread++) 
		pthread_join(thread_handles[thread], NULL); 
	
	//==============================================================

	toc();

	//Output results to 'argv[4]'.txt
	fprintf(outfile, "%i\n", Samples);
	for (index=0;index<Samples;++index)
	{
		fprintf(outfile, "%i\n", sampleClusterMap[index]);
	}

	//printf("Completed!\nTime: %lf\n", etime());
	printf("%lf\n", etime());

	return 0;

}

void *kMeans(void* rank)
{
	long threadRank=(long)rank;

	long workSize=Samples/threadCount;

	long threadStart=threadRank *workSize;

	long threadEnd=(threadRank+1)*workSize-1;

	int i;

	int position;

	int y;

	int z;

	double Distance;

	double kMeansBufferCalculation;

	double minimumDistance=1000000;

	//Last one deals with the rest
	if(threadRank==(threadCount-1))
	{
		threadEnd=Samples - 1;
	}


	/*the massive, one-hundred times loop starts here*/
	for(z=0; z<100; ++z)
	{
		/*first thread resets each element in each cluster*/
		if(threadRank==0)
		{
			for(i=0; i<K;++i)
			{
				clusterElements[i]=0;
			}
		}

		int barrierResult=pthread_barrier_wait(&kmeansBarrier);

		if(barrierResult!=0 && barrierResult!=PTHREAD_BARRIER_SERIAL_THREAD)
	    	{
	       	printf("Uh-oh, kmeansBarrier error\n");

	       	exit(0);
	   	}

	   	/*assign each element in the dataset to the cluster that it is closest to*/
		for (i=threadStart;i<=threadEnd;++i)
		{

			/*go through all the clusters*/
			for(position=0;position<K;++position)
			{

				kMeansBufferCalculation=0;

				/*calculate the distance in n-dimensions*/
				for(y=0; y<Dimensions;++y)
				{

					kMeansBufferCalculation=kMeansBufferCalculation+pow((clusterCoordinateInfo[position][y] - data[i][y]), 2);
				}

				Distance=sqrt(kMeansBufferCalculation);

				/*if the distance is small enough, assign data point to appropriate cluster*/
				if (Distance < minimumDistance)
				{
					minimumDistance=Distance;

					sampleClusterMap[i]=position;
				}
				
			}

			/*stop the loop if we can't find a sample that needs to be moved to a different cluster*/
			if(sampleClusterMap[i]!=priorSampleClusterMap[i])
			{
				flagOne=0;
			}

			/*update priorSampleClusterMap for the next iteration*/
			priorSampleClusterMap[i]=sampleClusterMap[i];

			//Update the number of elements in each cluster
			pthread_mutex_lock(&kmeansMutex);

			clusterElements[sampleClusterMap[i]]++;

			pthread_mutex_unlock(&kmeansMutex);

			minimumDistance=1000000;

		}

		barrierResult=pthread_barrier_wait(&kmeansBarrier);

		if(barrierResult!=0 && barrierResult!=PTHREAD_BARRIER_SERIAL_THREAD)
	    	{
	        	printf("Uh-oh, kmeansBarrier error\n");

	        	exit(0);
	    	}

	    	/*only the thread with rank equal to zero*/
		if(threadRank==0)
		{
			for (i=0;i<K;++i)
			{
				printf("Cluster %i contains: %i elements\n", i, clusterElements[i]); //Uncomment for tracing

				for(position=0;position<Dimensions;++position)
				{
					clusterCoordinateInfo[i][position] = 0;
				}
			}
		}

		barrierResult=pthread_barrier_wait(&kmeansBarrier);

		if(barrierResult!=0 && barrierResult!=PTHREAD_BARRIER_SERIAL_THREAD)
	    	{
	        	printf("kmeansBarrier error\n");

	        	exit(0);
	    	}	


		//Update the center of cluster by calculating the mean of the elements within each cluster*/
		for(position=threadStart;position<=threadEnd;++position)
		{
			pthread_mutex_lock(&kmeansMutex);

			for(y=0;y<Dimensions;++y)
			{
				clusterCoordinateInfo[sampleClusterMap[position]][y]=clusterCoordinateInfo[sampleClusterMap[position]][y] + data[position][y];
			}

			pthread_mutex_unlock(&kmeansMutex);		
		}

		barrierResult=pthread_barrier_wait(&kmeansBarrier);

		if(barrierResult!=0 && barrierResult!=PTHREAD_BARRIER_SERIAL_THREAD)
	    	{
	        	printf("Uh-oh, kmeansBarrier error\n");

	        	exit(0);
	    	}

	    /*divide sum by number of data elements*/
	    if(threadRank==0)
	    {
			for(i=0;i<K;++i)
			{
		    		printf("Cluster %i: ", i); //Uncomment for tracing

				for(y=0;y<Dimensions;++y)
				{
					/*we might have an empty cluster since we are dealing with random assignment*/
					if(clusterCoordinateInfo[i][y]!=0)
					{
						clusterCoordinateInfo[i][y]=clusterCoordinateInfo[i][y] / (double)clusterElements[i];
					}

					printf("%lf ", clusterCoordinateInfo[i][y]); //Uncomment for tracing
				}

				printf("\n"); //Uncomment for tracing
			}		
		}

		if(threadRank==0)
		{
			printf("--------------------ITERATION %i--------------------------\n", z + 1);
		}

		/*checks to see if we need to stop the 100 times loop*/
		pthread_mutex_lock(&kmeansMutex);

		flagTwo=(flagTwo & flagOne);

		pthread_mutex_unlock(&kmeansMutex);

		barrierResult=pthread_barrier_wait(&kmeansBarrier);

		if(barrierResult!=0 && barrierResult!=PTHREAD_BARRIER_SERIAL_THREAD)
	   	{
	        	printf("kmeansBarrier error\n");

	        	exit(0);
	    	}

	      /*if no more samples move clusters, we are done*/
	      if(flagTwo==1)
	      {

			break;
	      } 

		barrierResult=pthread_barrier_wait(&kmeansBarrier);

		if(barrierResult!=0 && barrierResult!=PTHREAD_BARRIER_SERIAL_THREAD)
	    	{
	        	printf("kmeansBarrier error\n");

	        	exit(0);
	    	}

		flagOne=1;

		flagTwo=1;


	} //threadEnd mapositionor loop

	return NULL;

}