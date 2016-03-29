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

	char *motifsArray;

	int arraySize;

	string motifs {};

	vector<string> sequences;

	//if(argc!=4)
	if(argc!=2)
	{
		cout << "Wrong amount of command line arguments. Should be <executable>  <motifFile>  <sequencesFile>  <outputFile>" << endl;

		cout << endl;

		cout << "The program will gracefully terminate, now" << endl;

		cout << endl;

		exit(0);
	}

	string motifLine;

	int motifLength;

	int numberOfMotifs;

	if(myRank==0)
	{
		motifFile.open(argv[1]);

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

				cout << "Moftif length: " << motifLength << endl;
			}
			else
			{
				//motifs.push_back(motifLine);
				motifs+=motifLine;
			}

			motifCounter++;
		}

		arraySize=numberOfMotifs*motifLength;

		motifsArray=new char[arraySize];

		for(int index=0;index<motifs.size();++index)
		{
			motifsArray[index]=motifs[index];
		}

		cout << "Processor " << myRank << " is going to send the following " << motifs << " motifs to other processors:" << endl;

		cout << endl;

		//send arraySize, the number of motifs, and the length of a motif to other processors so they can use it
		MPI_Send(&arraySize, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		MPI_Send(&numberOfMotifs, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		MPI_Send(&motifLength, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		//send the motif array. need arraySize+1 to account for the \0 character.
		MPI_Send(motifsArray, arraySize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

	}
	else if(myRank==1)
	{
		MPI_Recv(&arraySize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		MPI_Recv(&numberOfMotifs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Recv(&motifLength, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		char motifsArray[arraySize];

		MPI_Recv(motifsArray, arraySize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		cout << "Processor " << myRank << " received array of size: " << arraySize << endl;

		cout << endl;

		cout << "Processor " << myRank << " received the following motifs (as one big string):" << endl;

		for(int count=0;count<arraySize;++count)
		{
			cout << motifsArray[count] << " ";
		}

		cout << endl;
	}

	delete [] motifsArray;

	MPI_Finalize();
}	