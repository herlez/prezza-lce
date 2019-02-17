#include <fstream>
#include <iostream> 
#include <cstdlib>
#include <inttypes.h>
#include <sys/time.h>
#include <array>
#include <algorithm>
#include <vector>
#include "util.hpp"
#include "lce.hpp"


using namespace std;

lce::lceDataStructure dataLCE;
char * dataRaw;
uint64_t dataSize;

vector<unsigned long> suf1;
vector<unsigned long> suf2;



struct less_than_key
{
    inline bool operator() (const unsigned long a, const unsigned long b)
    {
		unsigned long i = a;
		unsigned long j = b;
		if (i == j) {
			return true;
		}
		
		while(i < dataSize && j < dataSize) {
			//cout << i << "  " << j << endl;
			if (dataRaw[i] == dataRaw[j]) {
				i++; j++;
			} else {
				return (dataRaw[i] < dataRaw[j]);
			}
		}
		return i > j;
    }
};



struct less_than_lce
{
	inline bool operator() (const unsigned long a, const unsigned long b)
	{
		uint64_t lce  = lce::fastlce(&dataLCE, a, b);
		if (a + lce >= dataLCE.numberOfBytes) { return true; }
		if (b + lce >= dataLCE.numberOfBytes) { return false; }
		return lce::getChar(&dataLCE, a+lce) < lce::getChar(&dataLCE, b+lce);
	}
};



double timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}





int main(int argc, char *argv[]) {
	
	ofstream log("../test/sssTest.txt", ios::out|ios::trunc);
    //cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	
	string fileNames[] {"english", "proteins", "dna",  "a"};
	string files[] = {"../text/english", "../text/proteins", "../text/dna", "../text/a"};
	bool testFinished[] = {false, false, false, false};
	
	 
	double ts1 = 0, ts2 = 0, ts3 = 0, ts4 = 0;//timestamps
	srand(0);
	
	
	for(unsigned int fileNumber = 0; fileNumber < 4; fileNumber++) {	
		/* Load LCE data structure */	
		string filePath = files[fileNumber];
		ifstream inputStream (filePath, ios::in|ios::binary);
		util::inputErrorHandling(&inputStream);
		dataSize = util::calculateSizeOfInputFile(&inputStream);
		
		/* Load raw text */
		log << "Data size: " << dataSize << endl;
		dataRaw = new char[dataSize];
		inputStream.seekg(0);
		inputStream.read(dataRaw, dataSize);
		lce::buildLCEDataStructure(&dataLCE, filePath, dataSize);
		
		
		ts1 = ts2 = ts3 = ts4 = 0; 
		for(unsigned long i = pow(2, 5); i < pow(2, 30) && !testFinished[fileNumber]; i = i*2)
		{
			if (i > dataSize) {
				i = dataSize;
				testFinished[fileNumber] = true;
			}
			
				
				

			suf1.resize(i);
			suf2.resize(i);
			log << suf1.size() << endl;;
			//suf2.resize(i);
			//generate random indexes
			for(unsigned long j = 0; j < i; j++) {
				suf1[j] = (j*dataSize)/i; // Random index
				suf2[j] = suf1[j];
			}
			 
			//Sort, using std::sort
			
			
			if(ts2-ts1 < 30) {
				ts1 = timestamp();
				std::sort(suf1.begin(), suf1.end(), less_than_key());
				ts2 = timestamp();
				log << "RESULT"
						<< " text=" << fileNames[fileNumber]
						<< " size=" << dataSize
						<< " suffixes=" << i
						<< " algo=" << "std::sort_naive"
						<< " time=" << ts2-ts1
						<< endl;
			}

			
			//Sort, using fastLCE queries
			
			if(ts4-ts3 < 30) {
				ts3 = timestamp();
				std::sort(suf2.begin(), suf2.end(), less_than_lce());
				ts4 = timestamp();
				log << "RESULT"
						<< " text=" << fileNames[fileNumber]
						<< " size=" << dataSize
						<< " suffixes=" << i
						<< " algo=" << "std::sort_fastlce"
						<< " time=" << ts4-ts3
						<< endl;
			}
			
			if(ts2-ts1 > 30 && ts4-ts3 > 30) {
				testFinished[fileNumber] = true;
			}
			
		//validate sss
		
		
		
		
		}
		delete[] dataLCE.fingerprints;
		delete[] dataRaw;		
	}
	return EXIT_SUCCESS;
	
	
}



