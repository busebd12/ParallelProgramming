#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <fstream>
#include <map>
#include <climits>
#include <cstring>
#include <iterator>
#include <sstream>
#include <mpi.h>
using namespace std;

void writeResults(const map<string, int> & FinalAnswer, ofstream & OutputFile)
{
	OutputFile << FinalAnswer.size() << endl;

	for(const auto & element : FinalAnswer)
	{
		OutputFile << element.first + ", " + to_string(element.second) << endl;
	}
}

void parsePartialResults(string & PartialResult, map<string, int> & FinalAnswer)
{
	auto lastQuestionMark=PartialResult.find_last_of("?");

	PartialResult.erase(++lastQuestionMark, string::npos);

	istringstream buffer(PartialResult);

	string token;

	while(getline(buffer, token, '?'))
	{
		string beforeNumbers=token.substr(0, 5);

		string numbersTillEnd=token.substr(5, string::npos);

		//if the motif has zero matches, just to be complete
		if(stoi(numbersTillEnd)==0)
		{
			FinalAnswer.insert({beforeNumbers, stoi(numbersTillEnd)});
		}
		else
		{
			FinalAnswer[beforeNumbers]++;
		}
	}

	for(const auto & motif : FinalAnswer)
	{
		cout << motif.first << ", " << motif.second << endl;
	}
}

string processorZerosWork(const string SequencesChunk, const string Motifs, const int NumberOfMotifs, const int SequencesLength, const int MotifLength)
{
	map<string, int> motifMap;

	cout << "Motifs: " << Motifs << endl;

	int start=0;

	int end=SequencesLength;

	while(end < SequencesChunk.size()+SequencesLength)
	{
		string currentSequence(&SequencesChunk[start], &SequencesChunk[end]);

		cout << endl;

		string repeatSequence {};

		for(int count=0;count<NumberOfMotifs;++count)
		{
			repeatSequence+=currentSequence;
		}

		int place=0;

		int matchCount=0;

		int count=1;

		//the motif we want to add to the map if everything matches
		string currentMotif {};

		//loop through all the sequences and compare the motif characters
		while(place < Motifs.size())
		{
			//keep adding a character to the current motif
			currentMotif.push_back(Motifs[place]);

			//matching character
			if((repeatSequence[place]==Motifs[place]) || (repeatSequence[place]!='X' && Motifs[place]=='X'))
			{
				matchCount++;
			}

			//we've reach the end of a sequence
			if(count%MotifLength==0)
			{
				//all the characters in the sequence match
				if(matchCount==SequencesLength)
				{
					motifMap[currentMotif]++;
				}

				//if there is a motif that doesn't match, just to be consistent with given output
				if(matchCount!=SequencesLength)
				{
					motifMap.insert({currentMotif, 0});
				}

				//clear so we can get it ready for the next motif
				currentMotif.clear();

				//need to reset since we will be starting to look at a new sequence
				matchCount=0;
			}

			place++;

			count++;
		}

		end+=MotifLength;

		start+=MotifLength;
	}

	string finalMotifs {};

	//loop through the map and combine all the motifs and the number of times they appear into one string
	for(const auto & motif : motifMap)
	{
		//add the question marks so we are trying to read our final result string,
		//we can know where one each result begins and ends
		finalMotifs+=(motif.first + to_string(motif.second) + "?");
	}

	return finalMotifs;
}

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

	if(argc!=4)
	{
		cout << "Wrong amount of command line arguments. Should be <executable>  <motifFile>  <sequencesFile>  <outputFile>" << endl;

		cout << endl;

		cout << "The program will gracefully terminate now" << endl;

		cout << endl;

		exit(0);
	}

	int motifLength;

	int sequencesLength;

	int numberOfMotifs;

	int numberOfSequences;

	int sequencesArraySize;

	int motifsArraySize;

	int chunkSize;

	int motifsPerProcessor;

	int sequencesPerProcessor;

	int finalSequencesArraySize;

	string motifLine;

	string sequencesLine;

	string motifs {};

	string sequences {};

	string chunks {};

	string finalResultString {};

	char *sequencesArray;

	char *motifsArray;

	char *motifChunks;

	char *sequenceChunks;

	char *finalSequencesArray;

	map<string, int> finalAnswer;

	if(myRank==0)
	{
		motifFile.open(argv[1]);

		sequencesFile.open(argv[2]);

		outputFile.open(argv[3]);

		if(!motifFile.is_open() || !sequencesFile.is_open() || !outputFile.is_open())
		{
			cout << "Couldn't open either the motif, sequences, or output file, you dumb fuck" << endl;

			cout << "Program will terminate gracefully now" << endl;

			exit(0);
		}

		int motifCounter=0;

		/////////////////////////////////////////////////////////////// START OF READING FROM FILES ///////////////////////////////////////////////////////

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

		if(motifLength!=sequencesLength)
		{
			cout << " *Facepalm* Motif length and sequences length are not the same" << endl;

			cout << "Program will exit gracefully now" << endl;

			exit(0);
		}

		///////////////////////////////////////////////////////////////////// END OF READING FROM FILES ////////////////////////////////////////////////////////

		cout << "Motifs: " << motifs << endl;

		cout << endl;

		cout << "Sequences: " << sequences << endl;

		cout << endl;

		motifsArraySize=motifs.size();

		cout << "motifsArraySize: " << motifsArraySize << endl;

		cout << endl;

		sequencesPerProcessor=numberOfSequences/numberOfProcessors;

		chunkSize=(sequencesPerProcessor*sequencesLength)+1;

		cout << "Chunk size: " << chunkSize << endl;

		cout << endl;

		//////////////////////////////////////////////////////////////////// START OF PROCESSOR ZERO'S WORK ///////////////////////////////////////////////////////////////////

		string processorZerosSequences=sequences.substr(0, chunkSize-1);

		cout << "processors zero's sequences: " << processorZerosSequences << endl;

		cout << endl;

		string processorZerosResult=processorZerosWork(processorZerosSequences, motifs, numberOfMotifs, sequencesLength, motifLength);

		cout << "processor zero's result: " << processorZerosResult << endl;

		cout << endl;

		///////////////////////////////////////////////////////////////// END OF PROCESSOR ZERO'S WORK ///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////// START OF OTHER PROCESSORS WORK ////////////////////////////////////////////////////////////////////

		sequenceChunks=new char[chunkSize];

		int current=chunkSize-1;

		int next=current+(chunkSize-1);

		//assign each processor (other than processor zero) their specific chunk of the sequences
		for(int processor=1;processor<numberOfProcessors;++processor)
		{
			//loop over sequences and chunk them according to the chunk size
			//while(next < motifs.size()+((chunkSize-1)*2))
			while(next < sequences.size()+((chunkSize-1)*2))
			{
				string assignedChunk(&sequences[current], &sequences[next]);

				cout << "Assigned sequence chunk for processor " << processor << " : " << assignedChunk << endl;

				cout << endl;

				strcpy(sequenceChunks, assignedChunk.c_str());

				//send the chunk
				MPI_Send(sequenceChunks, chunkSize, MPI_CHAR, processor, 0, MPI_COMM_WORLD);

				next+=chunkSize-1;

				current+=chunkSize-1;

				break;
			}
		}

		//send the chunkSize
		MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//send the size of each sequence
		MPI_Bcast(&sequencesLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//send the sequences array size
		//MPI_Bcast(&sequencesArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		MPI_Bcast(&motifsArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//copy the motifs into from the file into the char array
		motifsArray=new char[motifsArraySize];

		strcpy(motifsArray, motifs.c_str());

		//broadcast the motifs to all of the processors
		MPI_Bcast(motifsArray, motifsArraySize, MPI_CHAR, 0, MPI_COMM_WORLD);

		//send over the number of motifs
		MPI_Bcast(&numberOfMotifs, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//send over the length of a motif
		MPI_Bcast(&motifLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

		///////////////////////////////////////////////////////////////////////// END OF OTHER PROCESSORS WORK /////////////////////////////////////////////////////////////////////////

		parsePartialResults(processorZerosResult, finalAnswer);
		
		//go back over processors and get what work they have done
		for(int processor=1;processor<numberOfProcessors;++processor)
		{
			MPI_Recv(&finalSequencesArraySize, 1, MPI_INT, processor, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			finalSequencesArray=new char[finalSequencesArraySize];

			//receive stuff from worker processors
			MPI_Recv(finalSequencesArray, finalSequencesArraySize, MPI_CHAR, processor, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			//copy into temp string
			string temp(finalSequencesArray);

			cout << "Received: " << temp << " from processor " << processor << endl;

			cout << endl;

			parsePartialResults(temp, finalAnswer);

			delete [] finalSequencesArray;
		}

		writeResults(finalAnswer, outputFile);

		delete [] sequenceChunks;

		delete [] motifsArray;	
	}
	else
	{
		//map for hashing
		map<string, int> motifMap;

		MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//each processor receives the length of an individual sequence
		MPI_Bcast(&sequencesLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

		MPI_Bcast(&motifsArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		motifsArray=new char[motifsArraySize];

		MPI_Bcast(motifsArray, motifsArraySize, MPI_CHAR, 0, MPI_COMM_WORLD);

		MPI_Bcast(&numberOfMotifs, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//receive the length of a motif
		MPI_Bcast(&motifLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

		sequenceChunks=new char[chunkSize];

		MPI_Recv(sequenceChunks, chunkSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//convert to strings for ease of working with

		string localSequenceChunks(sequenceChunks);

		string localMotifs(motifsArray);

		int start=0;

		int end=sequencesLength;

		while(end < localSequenceChunks.size()+sequencesLength)
		{
			string currentSequence(&localSequenceChunks[start], &localSequenceChunks[end]);

			cout << endl;

			string repeatSequence {};

			for(int count=0;count<numberOfMotifs;++count)
			{
				repeatSequence+=currentSequence;
			}

			int place=0;

			int matchCount=0;

			int count=1;

			//the motif we want to add to the map if everything matches
			string currentMotif {};

			//loop through all the sequences and compare the motif characters
			while(place < localMotifs.size())
			{
				//keep adding a character to the current motif
				currentMotif.push_back(localMotifs[place]);

				//matching character
				if((repeatSequence[place]==localMotifs[place]) || (repeatSequence[place]!='X' && localMotifs[place]=='X'))
				{
					matchCount++;
				}

				//we've reach the end of a sequence
				if(count%motifLength==0)
				{
					//all the characters in the sequence match
					if(matchCount==sequencesLength)
					{						
						motifMap[currentMotif]++;
					}

					//if there is a motif that doesn't match, just to be consistent with given output
					if(matchCount!=sequencesLength)
					{
						motifMap.insert({currentMotif, 0});
					}

					//clear so we can get it ready for the next motif
					currentMotif.clear();

					//need to reset since we will be starting to look at a new sequence
					matchCount=0;
				}

				place++;

				count++;
			}

			end+=motifLength;

			start+=motifLength;
		}

		string finalMotifs {};

		//loop through the map and combine all the motifs and the number of times they appear into one string
		for(const auto & motif : motifMap)
		{
			finalMotifs+=(motif.first + to_string(motif.second) + "?");
		}

		cout << "The final motif of size " << finalMotifs.size() << " to be sent back to the master processor from processor " << myRank << " : " << finalMotifs << endl;

		cout << endl;

		finalSequencesArraySize=finalMotifs.size();

		finalSequencesArray=new char[finalSequencesArraySize];

		//copy that one giant string we created eariler into a character array
		//so that we can actually send it back to the master processor
		for(int index=0;index<finalSequencesArraySize;++index)
		{
			finalSequencesArray[index]=finalMotifs[index];
		}

		//send the size of the final motifs array to the master processor
		MPI_Send(&finalSequencesArraySize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);

		//send the final motifs back to the master processor
		MPI_Send(finalSequencesArray, finalSequencesArraySize, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
	}

	motifFile.close();

	sequencesFile.close();

	outputFile.close();

	MPI_Finalize();
}	