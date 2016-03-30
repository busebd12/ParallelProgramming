#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <mpi.h>
#include <fstream>
using namespace std;

int main(int argc, char* argv [])
{
	MPI_Init(NULL, NULL);

	int commSize;

	int myRank;

	MPI_Comm_size(MPI_COMM_WORLD, &commSize);

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

	string motifLine;

	string sequencesLine;

	int motifLength;

	int sequencesLength;

	int numberOfMotifs;

	int numberOfSequences;

	char *motifsArray;

	char* sequencesArray;

	int motifArraySize;

	int sequencesArraySize;

	string motifs {};

	string sequences {};

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

		//need the extra 1 for the \0 character
		motifArraySize=(numberOfMotifs*motifLength)+1;

		sequencesArraySize=(numberOfSequences*sequencesLength)+1;

		motifsArray=new char[motifArraySize];

		sequencesArray=new char[sequencesArraySize];

		for(int index=0;index<motifs.size();++index)
		{
			motifsArray[index]=motifs[index];
		}

		for(int spot=0;spot<sequences.size();++spot)
		{
			sequencesArray[spot]=sequences[spot];
		}

		cout << "Processor " << myRank << " is going to send the following " << motifs << " motifs to other processors:" << endl;

		cout << endl;

		cout << "Processor " << myRank << " is going to send the following " << sequences << " sequences to the other processors" << endl;

		cout << endl;

		//send motifArraySize, the number of motifs, and the length of a motif to other processors so they can use it
		MPI_Send(&motifArraySize, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		MPI_Send(&numberOfMotifs, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		MPI_Send(&motifLength, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		//send the motif array
		MPI_Send(motifsArray, motifArraySize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

		//send sequencesArraySize, number of sequences, and length of a sequence to other processors so they can use it
		MPI_Send(&sequencesArraySize, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		MPI_Send(&numberOfSequences, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		MPI_Send(&sequencesLength, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		//send the sequences array
		MPI_Send(sequencesArray, sequencesArraySize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

		delete [] motifsArray;

		delete [] sequencesArray;

	}
	else if(myRank==1)
	{
		MPI_Recv(&motifArraySize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		MPI_Recv(&numberOfMotifs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Recv(&motifLength, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		char motifsArray[motifArraySize];

		MPI_Recv(motifsArray, motifArraySize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


		MPI_Recv(&sequencesArraySize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		MPI_Recv(&numberOfSequences, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Recv(&sequencesLength, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		char sequencesArray[sequencesArraySize];

		MPI_Recv(sequencesArray, sequencesArraySize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		string matchingMotifs {};

		int current=0;

		int next=motifLength;

		while(next < motifArraySize+motifLength)
		{
			string currentMotif(&motifsArray[current], &motifsArray[next]);

			string repeatMotif {};

			//construct string the same size of the sequences array made
			//up of only the motif we want to use to compare
			for(int i=0;i<numberOfSequences;++i)
			{
				repeatMotif+=currentMotif;
			}

			int place=0;

			int matchCount=0;

			int count=1;

			cout << "Using motif: " << currentMotif << endl;

			while(place < sequencesArraySize)
			{
				cout << sequencesArray[place] << " and " << motifsArray[place] << " ";

				cout << endl;

				//matching character
				if((repeatMotif[place]==sequencesArray[place]) || (sequencesArray[place]!='X' && repeatMotif[place]=='X'))
				{
					matchCount++;
				}

				//we've reach the end of a sequence
				if(count%sequencesLength==0)
				{
					//all the characters in the sequence match
					if(matchCount==sequencesLength)
					{
						cout << "Adding the motif: " << currentMotif << endl;

						matchingMotifs+=currentMotif;
					}

					//need to reset since we will be starting to look at a new sequence
					matchCount=0;
				}

				place++;

				count++;
			}

			cout << endl;

			cout << endl;

			next+=motifLength;

			current+=motifLength;
		}

		cout << "The matching motifs were: " << endl;

		int present=0;

		int future=motifLength;

		while(future < matchingMotifs.size()+motifLength)
		{
			string motif(&matchingMotifs[present], &matchingMotifs[future]);

			cout << motif << endl;

			present+=motifLength;

			future+=motifLength;
		}
	}

	motifFile.close();

	sequencesFile.close();

	MPI_Finalize();
}	