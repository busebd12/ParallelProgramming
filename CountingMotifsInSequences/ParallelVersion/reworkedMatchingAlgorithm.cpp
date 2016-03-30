#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <vector>
using namespace std;

int main() 
{
   string sequences={"GRCATCKCCRGTCATTSCSAASCGTCKCCLCKCLMCLCCK"};
   
   string motifs={"XRCATTXCXACXCCXKXRXK"};
   
   vector<string> matchingMotifs;
   
   int motifLength=5;
   
   int sequenceLength=5;
   
   int numberOfSequences=8;
  
  int current=0;
  
  int next=motifLength;
  
  while(next < motifs.size()+motifLength)
  {
      string temp(&motifs[current], &motifs[next]);
      
      string repeatMotif {};
      
      for(int i=0;i<numberOfSequences;++i)
      {
          repeatMotif+=temp;
      }
      
      int place=0;
      
      int matchCount=0;
      
      int count=1;
      
      //cout << "Going to compare: " << endl;
      
      //cout << "Sequences: " << sequences << endl;
      
      //cout << "Motifs: " << repeatMotif << endl;
      
      cout << "Using motif: " << temp << endl;
  
    while(place < sequences.size())
    {
        cout << sequences[place] << " and " << repeatMotif[place] << " ";

        cout << endl;       

       if((repeatMotif[place]==sequences[place]) || (sequences[place]!='X' && repeatMotif[place]=='X'))
       {
            matchCount++;
       }

        if(count%sequenceLength==0)
        {
            cout << "End of a sequence" << endl;

            cout << "matchCount: " << matchCount << endl;  

            if(matchCount==sequenceLength /*|| matchCount!=sequenceLength*/)
            {
                cout << "Adding the motif: " << temp << endl;

                matchingMotifs.push_back(temp);
            }

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
  
  cout << "The matching motifs were:" << endl;
  
  for(const auto & motif : matchingMotifs)
  {
      cout << motif << endl;
  }
  
   
}

