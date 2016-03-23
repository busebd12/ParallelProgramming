#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
using namespace std;

void searchForInstances(const vector<string> & Motifs, const vector<string> & Sequences, int MotifLength, ofstream & OutputFile)
{
	int index=0;

	map<string, int> motifMap;

	do
	{
		string sequence=Sequences[index];

		for(int position=0;position<Motifs.size();++position)
		{
			int matchCount=0;

			for(int spot=0;spot<sequence.size();++spot)
			{
				if(sequence[spot]==Motifs[position][spot] || (Motifs[position][spot]=='X' && sequence[spot]!='X'))
				{
					matchCount++;
				}
			}

			if(matchCount==sequence.size())
			{
				motifMap[Motifs.at(position)]++;
			}

			if(matchCount!=sequence.size())
			{
				motifMap.insert({Motifs.at(position), 0});
			}
		}

		index++;
	}
	while(index!=Sequences.size());

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

	vector<string> motifs;

	vector<string> sequences;

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
			motifLength=motifLine[2]-'0';
		}

		if(motifCounter!=0)
		{
			motifs.push_back(motifLine);
		}

		motifCounter++;
	}

	int sequencesCounter=0;

	//read stuff from the sequences file
	while(getline(sequencesFile, sequencesLine))
	{
		if(sequencesCounter==0)
		{
			sequencesLength=sequencesLine[2]-'0';
		}

		if(sequencesCounter!=0)
		{
			sequences.push_back(sequencesLine);
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

	searchForInstances(motifs, sequences, motifLength, outputFile);

	motifFile.close();

	sequencesFile.close();

	outputFile.close();
}