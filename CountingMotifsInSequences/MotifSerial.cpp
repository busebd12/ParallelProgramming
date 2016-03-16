#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
using namespace std;

void searchForInstances(const vector<string> & Motifs, const vector<string> & Sequences, int MotifLength)
{
	int index=0;

	map<string, int> motifMap;

	do
	{
		string sequence=Sequences[index];

		int matchCount=0;

		for(int position=0;position<Motifs.size();++position)
		{
			for(int spot=0;spot<Motifs[position].size();++spot)
			{
				if(sequence[spot]==Motifs[position][spot] || (Motifs[position][spot]=='X' && sequence[spot]!='X'))
				{
					//cout << "Motif character: " << Motifs[position][spot] << endl;

					//cout << "Sequence character: " << sequence[spot] << endl;

					matchCount++;
				}
			}

			if(matchCount==Motifs[position].size())
			{
				motifMap[Motifs.at(position)]++;
			}
		}

		index++;
	}
	while(index!=Sequences.size());

	for(const auto & element : motifMap)
	{
		cout << "Motif: " << element.first << " count: " << element.second << endl;
	}

	cout << endl;
}

int main(int argc, char* argv [])
{
	ifstream motifFile;

	ifstream sequencesFile;

	vector<string> motifs;

	vector<string> sequences;

	motifFile.open(argv[1]);

	sequencesFile.open(argv[2]);

	int motifLength;

	int sequencesLength;

	//check to see if we can open either of the files. if not, exit the program gracefully.
	if(!motifFile.is_open() || !sequencesFile.is_open())
	{
		cout << "Couldn't find either the motif file or the sequences file" << endl;

		cout << endl;

		cout << "The program will gracefully terminate" << endl;

		exit(0);
	}

	string motifLine;

	string sequencesLine;

	cout << "Reading from the motif file:" << endl;

	int motifCounter=0;

	while(getline(motifFile, motifLine))
	{
		cout << "Line from motif file: " << motifLine << endl;

		cout << endl;

		if(motifCounter==0)
		{
			//cout << "Motif length: " << motifLine[2]-'0' << endl;

			//cout << endl;

			motifLength=motifLine[2]-'0';
		}

		if(motifCounter!=0)
		{
			motifs.push_back(motifLine);
		}

		motifCounter++;
	}

	cout << "Reading from the sequences file:" << endl;

	int sequencesCounter=0;

	while(getline(sequencesFile, sequencesLine))
	{
		cout << "Line from sequences file: " << sequencesLine << endl;

		cout << endl;

		if(sequencesCounter==0)
		{
			//cout << "Sequnce length: " << sequencesLine[2]-'0' << endl;

			//cout << endl;

			sequencesLength=sequencesLine[2]-'0';
		}

		if(sequencesCounter!=0)
		{
			sequences.push_back(sequencesLine);
		}

		sequencesCounter++;
	}

	if(motifLength!=sequencesLength)
	{
		cout << "The motif and sequence lengths don't match" << endl;

		cout << endl;

		cout << "The program with exit gracefully now" << endl;

		exit(0);
	}

	searchForInstances(motifs, sequences, motifLength);
}