#include <fstream>
#include <iostream> 
#include <cstdlib>
#include <inttypes.h>
#include <sys/time.h>
#include <array>
#include <algorithm>
 
#include "util.hpp"
#include "lce.hpp"

//#define consoleIO

using namespace std;

lce::lceDataStructure dataLCE;
uint64_t size = 1000000000ULL;
char * dataRaw = new char[size];



struct less_than_key
{
    inline bool operator() (const uint64_t a, const uint64_t b)
    {
		uint64_t i = a;
		uint64_t j = b;
		if (i == j) {
			return true;
		}
		
		while(i < size && j < size) {
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
	inline bool operator() (const uint64_t a, const uint64_t b)
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
	
	ofstream log("sss.txt", ios::out|ios::trunc);
    //cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	
	string fileNames[] {"english", "proteins", "a"};
	string files[] = {"../text/english", "../text/proteins", "../text/a"}; 
	double ts1, ts2;//timestamps
	/* Beforehand generate random numbers */
	srand(0);
	
	cout << "ok" << endl;
	/* Do the SSS test */
	//std::array<uint64_t ,10000000>* suf1 = new array<uint64_t, 10000000>;
	//std::array<uint64_t ,10000000>* suf2 = new array<uint64_t, 10000000>;
	
	//for(uint64_t i = 0; i < suffixes1.size(); i++) {
	//	suffixes1[i] = util::randomIndex(1000000000ULL); // Indexes up to 10^9
	//	suffixes2[i] = suffixes1[i];
	//}

	for(unsigned int fileNumber = 0; fileNumber < 2; fileNumber++) {		
		string filePath = files[fileNumber];
		ifstream inputStream (filePath, ios::in|ios::binary);
		util::inputErrorHandling(&inputStream);
		
		inputStream.seekg(0);
		inputStream.read(dataRaw, size);
		lce::buildLCEDataStructure(&dataLCE, filePath, size);
		
		constexpr uint64_t sizes[] = {1000000};
		for(int i = 0; i < 1; i++)
		{
			//generate random indexes
			cout << "lul"<< endl;;
			std::array<uint64_t ,sizes[0]> suf1;
			std::array<uint64_t ,sizes[0]> suf2;
			cout << "lul";
	
			for(uint64_t i = 0; i < suf1.size(); i++) {
				suf1[i] = util::randomIndex(1000000000ULL); // Indexes up to 10^9
				suf2[i] = suf1.at(i);
			}
			
			//Sort, using std::sort
			
			ts1 = timestamp();
			std::sort(suf1.begin(), suf1.end(), less_than_key());
			ts2 = timestamp();
			log << "RESULT"
					<< " text=" << fileNames[fileNumber]
					<< " size=" << size
					<< " suffixes=" << suf1.size()
					<< " algo=" << "std::sort_naive"
					<< " time=" << ts2-ts1
					<< endl;
			
			
			
			//Sort, using fastLCE queries
			
			
			ts1 = timestamp();
			std::sort(suf2.begin(), suf2.end(), less_than_lce());
			ts2 = timestamp();
			log << "RESULT"
					<< " text=" << fileNames[fileNumber]
					<< " size=" << size
					<< " suffixes=" << suf2.size()
					<< " algo=" << "std::sort_fastlce"
					<< " time=" << ts2-ts1
					<< endl;
		
		
		delete[] &suf1;
		delete[] &suf2;

		}		
	}
	return EXIT_SUCCESS;
	
	
}



