#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <fstream>
#include <map>
#include <climits>
#include <cstring>
#include <mpi.h>
using namespace std;

int main(int argc, char* argv [])
{
	MPI_Init(&argc, &argv);

	int numberOfProcessors;

	int myRank;

	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcessors);

	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	ifstream motifFile;

	ifstream sequencesFile;

	ofstream outputFile;

	//if(argc!=4)
	if(argc!=3)
	{
		cout << "Wrong amount of command line arguments. Should be <executable>  <motifFile>  <sequencesFile>  <outputFile>" << endl;

		cout << endl;

		cout << "The program will gracefully terminate, now" << endl;

		cout << endl;

		exit(0);
	}

	int motifLength;

	int sequencesLength;

	int numberOfMotifs;

	int numberOfSequences;

	int sequencesArraySize;

	int chunkSize;

	int motifsPerProcessor;

	string motifLine;

	string sequencesLine;

	string motifs {};

	string sequences {};

	string chunks {};

	string finalResultString {};

	char *sequencesArray;

	char *motifChunks;

	if(myRank==0)
	{
		motifFile.open(argv[1]);

		sequencesFile.open(argv[2]);

		int motifCounter=0;

		//read stuff from the motif file
		while(getline(motifFile, motifLine))
		{
			if(motifCounter==0)
			{
				string space {" "};

				//find the space since the size of the motif will always follow the space character
				auto foundSpace=motifLine.find(space);

				//form a substring of everything after the space (i.e. the size of the motif)
				string substringWithSize=motifLine.substr(++foundSpace);

				string motifNumber=motifLine.substr(0, foundSpace);

				//convert string verison of size to integer
				motifLength=stoi(substringWithSize);

				numberOfMotifs=stoi(motifNumber);

				cout << "Number of motifs: " << numberOfMotifs << endl;

				cout << endl;

				cout << "Moftif length: " << motifLength << endl;

				cout << endl;
			}
			else
			{
				//motifs.push_back(motifLine);
				motifs+=motifLine;
			}

			motifCounter++;
		}

		int sequencesCounter=0;

		//read stuff from the sequences file
		while(getline(sequencesFile, sequencesLine))
		{
			if(sequencesCounter==0)
			{
				string space {" "};

				//find the space since the size of the sequence will always follow the space character
				auto foundSpace=sequencesLine.find(space);

				//form a substring of everything after the space (i.e. the size of the sequence)
				string substringWithSize=sequencesLine.substr(++foundSpace);

				string sequencesNumber=sequencesLine.substr(0, foundSpace);

				//convert string verison of size to integer
				sequencesLength=stoi(substringWithSize);

				numberOfSequences=stoi(sequencesNumber);

				cout << "Sequence length: " << sequencesLength << endl;

				cout << endl;

				cout << "Number of sequences: " << numberOfSequences << endl;

				cout << endl;
			}

			if(sequencesCounter!=0)
			{
				sequences+=sequencesLine;
			}

			sequencesCounter++;
		}

		cout << "Motifs: " << motifs << endl;

		cout << endl;

		cout << "Sequences: " << sequences << endl;

		cout << endl;

		sequencesArraySize=numberOfMotifs*motifLength;

		//cout << "Number of processors: " << numberOfProcessors << endl;

		//cout << "Number of sequences: " << numberOfSequences << endl;

		motifsPerProcessor=numberOfMotifs/numberOfProcessors;

		chunkSize=motifsPerProcessor*motifLength;

		cout << "Chunk size: " << chunkSize << endl;

		cout << endl;

		motifChunks=new char[chunkSize];

		int next=chunkSize;

		int current=0;

		//assign each processor their specific chunk of the sequences
		for(int processor=0;processor<numberOfProcessors;++processor)
		{
			//loop over sequences and chunk them according to the chunk size
			while(next < motifs.size()+chunkSize)
			{
				string assignedChunk(&motifs[current], &motifs[next]);

				cout << "Assigned motif chunk: " <<  assignedChunk << endl;

				strcpy(motifChunks, assignedChunk.c_str());

				cout << "motifChunks: " << motifChunks << endl;

				cout << "chunkSize: " << chunkSize << endl;

				//send the chunk
				//MPI_Send(motifChunks, chunkSize, MPI_CHAR, processor, 0, MPI_COMM_WORLD);

				next+=chunkSize;

				current+=chunkSize;

				break;
			}
		}

		/*
		sequencesArray=new char[sequencesArraySize];

		//broadcast the all of the motifs to all of the processors
		MPI_Bcast(sequencesArray, sequencesArraySize, MPI_CHAR, 0, MPI_COMM_WORLD);

		//send the size of each sequence
		MPI_Bcast(&sequencesLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//send the sequences array size
		MPI_Bcast(&sequencesArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//send over the number of motifs
		MPI_Bcast(&numberOfSequences, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//send over the length of a motif
		MPI_Bcast(&motifLength, 1, MPI_INT, 0, MPI_COMM_WORLD);
		*/

		/*
		int finalsequencesArraySize;

		char finalsequencesArray[finalsequencesArraySize];

		//go back over processors and get what work they have done
		for(int processor=0;processor<numberOfProcessors;++processor)
		{
			//receive stuff from worker processors
			MPI_Recv(finalsequencesArray, finalsequencesArraySize, MPI_CHAR, processor, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			//copy into temp string
			string temp(finalsequencesArray);

			//add temp to final string result
			finalResultString+=temp;
		}
		*/

		//print result


		//free the electrons
		delete [] motifChunks;

		//delete [] sequencesArray;
		
	}

	else
	{
		//map for hashing
		map<string, int> motifMap;

		//each processor receives their respective chunks
		MPI_Recv(motifChunks, chunkSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		/*
		//each processor receives the length of an individual sequence
		MPI_Bcast(&sequencesLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//receive sequences array size
		MPI_Bcast(&sequencesArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//receive the number of sequences
		MPI_Bcast(&numberOfSequences, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//receive the length of a motif
		MPI_Bcast(&motifLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

		sequencesArray=new char[sequencesArraySize];

		//receive all the sequences
		MPI_Bcast(sequencesArray, sequencesArraySize, MPI_CHAR, 0, MPI_COMM_WORLD);
		*/

		//convert to strings for ease of working with
		string localMotifChunks(motifChunks);

		//string localSequences(sequencesArray);

		cout << "localMotifChunks: " << localMotifChunks << endl;

		//cout << endl;

		//cout << "localSequences: " << localSequences << endl;

		/*
		int current=0;

		int next=motifLength;

		//loop through the subset of the motifs
		while(next < localMotifChunks.size()+motifLength)
		{
			//get the current motif we want to work with
			string currentMotif(&localMotifChunks[current], &localMotifChunks[next]);

			string repeatMotif {};

			
			//create a string made up of the same motif that is equal to the length
			//of the sequences string so we can compare each motif against every sequence
			
			for(int count=0;count<numberOfSequences;++count)
			{
				repeatMotif+=currentMotif;
			}

			int place=0;

			int matchCount=0;

			int count=1;

			//loop through all the sequences and compare the motif characters
			while(place < localSequences.size())
			{
				//cout << sequencesArray[place] << " and " << sequencesArray[place] << " ";

				//cout << endl;

				//matching character
				if((localSequences[place]==repeatMotif[place]) || (localSequences[place]!='X' && repeatMotif[place]=='X'))
				{
					matchCount++;
				}

				//we've reach the end of a sequence
				if(count%sequencesLength==0)
				{
					//all the characters in the sequence match
					if(matchCount==sequencesLength)
					{
						//cout << "Adding the motif: " << currentMotif << endl;
						
						//matchingMotifs+=currentMotif;
						motifMap[currentMotif]++;
					}

					//if there is a motif that doesn't match, just to be consistent with given output
					if(matchCount!=sequencesLength)
					{
						motifMap.insert({currentMotif, 0});
					}

					//need to reset since we will be starting to look at a new sequence
					matchCount=0;
				}

				place++;

				count++;
			}

			//cout << endl;

			//cout << endl;

			next+=motifLength;

			current+=motifLength;
		}


		string finalMotifs {};

		//loop through the map and combine all the motifs and the number of times they appear into one string
		for(const auto & motif : motifMap)
		{
			finalMotifs+=(motif.first + to_string(motif.second));
		}

		//cout << endl;

		//cout << "The final motif to be sent back to the master processor: " << finalMotifs << endl;

		//cout << endl;

		int finalsequencesArraySize=finalMotifs.size();

		//cout << "finalsequencesArraySize on the slave(s) processor: " << finalsequencesArraySize << endl;

		char finalsequencesArray[finalsequencesArraySize];

		
		//copy that one giant string we created eariler into a character array
		//so that we can actually send it back to the master processor
		for(int index=0;index<finalMotifs.size();++index)
		{
			finalsequencesArray[index]=finalMotifs[index];
		}

		//cout << "Before sending from slave processor" << endl;

		//send the size of the final motifs array to the master processor
		MPI_Send(&finalsequencesArraySize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);

		//send the final motifs back to the master processor
		MPI_Send(finalsequencesArray, finalsequencesArraySize, MPI_CHAR, 0, 1, MPI_COMM_WORLD);

		*/
		//free the electrons
		//delete [] sequencesArray;
		
	}

	motifFile.close();

	sequencesFile.close();

	MPI_Finalize();
}	