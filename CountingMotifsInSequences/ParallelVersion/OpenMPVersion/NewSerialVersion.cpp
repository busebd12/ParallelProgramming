#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include "etime.h"
using namespace std;

void checkForMatches(const string & Motifs, const string & Sequences, const int & MotifLength, const int & SequencesLength, const int & NumberOfSequences, ofstream & OutputFile)
{
	map<string, int> motifMap;

	int motifLength=MotifLength;

	int sequenceLength=SequencesLength;

	int numberOfSequences=NumberOfSequences;

	int current {};

	int next=motifLength;

	while(next < Motifs.size()+motifLength)
	{
		string currentMotif(&Motifs[current], &Motifs[next]);

		string repeatMotif {};

		for(int i=0;i<NumberOfSequences;++i)
		{
			repeatMotif+=currentMotif;
		}

		int place=0;

		int matchCount=0;

		int count=1;

		cout << "Using motif: " << currentMotif << endl;

		while(place < Sequences.size())
		{
			cout << Sequences[place] << " and " << repeatMotif[place] << " ";

			cout << endl;

			if((repeatMotif[place]==Sequences[place]) || (Sequences[place]!='X' && repeatMotif[place]=='X'))
			{
				matchCount++;
			}

			if(count%SequencesLength==0)
			{
				cout << "End of a sequence" << endl;

				cout << "mathCount: " << matchCount << endl;

				if(matchCount==SequencesLength)
				{
					cout << "Adding the motif: " << currentMotif << endl;

					motifMap[currentMotif]++;
				}
				else
				{
					motifMap.insert({currentMotif, 0});
				}

				matchCount=0;
			}

			place++;

			count++;
		}

		cout << endl;

		cout << endl;

		next+=MotifLength;

		current+=MotifLength;
	}

	cout << motifMap.size() << endl;

	OutputFile << motifMap.size() << endl;

	for(const auto & element : motifMap)
	{
		cout << element.first << "," << element.second << endl;

		OutputFile << element.first + ", " + to_string(element.second) << endl;
	}

	cout << endl;
}

int main(int argc, char* argv [])
{
	ifstream motifFile;

	ifstream sequencesFile;

	ofstream outputFile;

	string motifs {};

	string sequences {};

	if(argc!=4)
	{
		cout << "Wrong amount of command line arguments. Should be <executable>  <motifFile>  <sequencesFile>  <outputFile>" << endl;

		cout << endl;

		cout << "The program will gracefully terminate, now" << endl;

		cout << endl;

		exit(0);
	}

	motifFile.open(argv[1]);

	sequencesFile.open(argv[2]);

	outputFile.open(argv[3]);

	int motifLength;

	int sequencesLength;

	int numberOfMotifs;

	int numberOfSequences;

	//check to see if we can open either of the files. if not, exit the program, gracefully
	if(!motifFile.is_open() || !sequencesFile.is_open() || !outputFile.is_open())
	{
		cout << "Couldn't find either the motif file, the sequences file, or the output file, go fuck yourself" << endl;

		cout << endl;

		cout << "The program will gracefully terminate now" << endl;

		exit(0);
	}

	string motifLine;

	string sequencesLine;

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

		if(motifCounter!=0)
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

	//check to see if the lengths match. If not, exit gracefully
	if(motifLength!=sequencesLength)
	{
		cout << "The motif and sequence lengths don't match" << endl;

		cout << endl;

		cout << "The program with exit gracefully now" << endl;

		exit(0);
	}

	tic();

	checkForMatches(motifs, sequences, motifLength, sequencesLength, numberOfSequences, outputFile);

	toc();

	cout << "Runtime: " << etime() << endl;

	motifFile.close();

	sequencesFile.close();

	outputFile.close();
}