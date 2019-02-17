#include <fstream>
#include <iostream> 
#include <cstdlib>
#include <inttypes.h>
#include <sys/time.h>


#include "util.hpp"
#include "lce.hpp"


//#define testConstruction
#define testLCE
//#define testAccess

using namespace std;

double timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int main(int argc, char *argv[]) {
	
	ofstream log("../test/time.txt", ios::out|ios::trunc);
	
	const string fileNames[] {"english", "a", "proteins", "dna"};
	const string files[] = {"../text/english", "../text/a", "../text/proteins", "../text/dna"};
	bool testFinished[] = {false, false, false, false};
	
	 
	double ts1, ts2, tsStart, tsEnd; //timestamps
	lce::lceDataStructure dataLCE;
	/* Beforehand generate random numbers */
	const uint64_t n = 10000000; //number of random numbers; 10000000 = 80MB
	uint64_t * rand = new uint64_t[n]; //array for random numbers
	srand(0);
	

	const uint64_t numberOfQueries = 10000000;
	
	tsStart = timestamp();
	/* MAIN TEST */
	for (uint64_t numberOfBytes = 1<<8; numberOfBytes <= 10000000000ULL; numberOfBytes = numberOfBytes*2) {
		
		
		for(unsigned int fileNumber = 0; fileNumber < 4 && !testFinished[fileNumber]; fileNumber++) {		
			
			
			string filePath = files[fileNumber];
			ifstream inputStream (filePath, ios::in|ios::binary);
			util::inputErrorHandling(&inputStream);
		
		/* Test with up to 8GB of data */
			uint64_t sizeInBytes = numberOfBytes; 
			if (numberOfBytes >= util::calculateSizeOfInputFile(&inputStream)) {
				sizeInBytes = util::calculateSizeOfInputFile(&inputStream);
				log << fileNames[fileNumber] << " finished." << endl;
				testFinished[fileNumber] = true;
				
			}
			//load random number which are used as indexes
			for(uint64_t i = 0; i < n; i++) {
				rand[i] = util::randomIndex(sizeInBytes);
			}
			
			//load Text
			ts1 = timestamp();
			char * dataRaw = new char[sizeInBytes];
			inputStream.seekg(0);
			inputStream.read(dataRaw, sizeInBytes);
			ts2 = timestamp();
#ifdef testConstruction
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "loadFile"
				<< " size=" << sizeInBytes
				<< " time=" << ts2-ts1
				<< endl;
#endif

			//build data structure
			ts1 = timestamp();
			
			lce::buildLCEDataStructure(&dataLCE, filePath, sizeInBytes);
			ts2 = timestamp();
#ifdef testConstruction
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "buildDataStructure"
				<< " size=" << sizeInBytes
				<< " time=" << ts2-ts1
				<< endl;
#endif
			
			
			
#ifdef testLCE
			//lce on LCE data structure
			uint64_t lceV = 0;
			ts1 = timestamp();
			for(uint64_t i = 0; i < numberOfQueries; i++) {
				lceV += lce::fastlce(&dataLCE, rand[i%n], rand[(i+1)%n]);
			}
			ts2 = timestamp();
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "fastLCE"
				<< " size=" << sizeInBytes
				<< " time=" << ts2-ts1
				<< endl 
				<< lceV //use lce so it does not get optimized
				<< endl;
			
			
			
			
			
			//lce on normal data
			lceV = 0;
			ts1 = timestamp();
			// NaiveLCE needs O(n) time, so we wont do it for big 'a' files:
			if(fileNumber != 1 || sizeInBytes <= pow(2, 14)) {
				for(uint64_t i = 0; i < numberOfQueries; i++) {
					uint64_t indexI = rand[i%n];
					uint64_t indexJ = rand[(i+1)%n];
					
					if(indexI == indexJ) {
						lceV += (sizeInBytes - indexI);
					} else {
						
						while (indexI < sizeInBytes && indexJ < sizeInBytes && dataRaw[indexI] == dataRaw[indexJ]) {
							indexI++;
							indexJ++;
							lceV++;
						}
					}
				}
				
				ts2 = timestamp();
				log << "RESULT"
					<< " text=" << fileNames[fileNumber]
					<< " algo=" << "naiveLCE"
					<< " size=" << sizeInBytes
					<< " time=" << ts2-ts1
					<< endl
					<< lceV
					<< endl;
			} else {
				log << "Too big for naiveLCE" << endl;
			}
#endif

#ifdef testAccess
			//test random access on LCE array
			char c = 0x00; //character to overwrite
			ts1 = timestamp();
			for(uint64_t i = 0; i < numberOfQueries*10; i++) {
				c ^= lce::getChar(&dataLCE, rand[i % n]);
			}
			ts2 = timestamp();
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "RA-LCE"
				<< " size=" << sizeInBytes
				<< " time=" << ts2-ts1
				<< endl
				<< (((unsigned int)c) % 146) + 20  // use c so it wont get optimized
				<< endl;

			// test random access on normal data
			c = 0x00;
			ts1 = timestamp();
			for(uint64_t i = 0; i < numberOfQueries*10; i++) {
				c ^= dataRaw[rand[i % n]];
			}
			ts2 = timestamp();
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "RA-RAW"
				<< " size=" << sizeInBytes
				<< " time=" << ts2-ts1
				<< endl
				<< (((unsigned int)c) % 146) + 20 // use c so it wont get optimized
				<< endl;
	
			// test sequential access  on lce data structure
			c = 0x00;
			ts1 = timestamp();
			for(uint64_t i = 0; i < numberOfQueries*10; i++) {
				c ^= lce::getChar(&dataLCE, i % numberOfBytes);
			}
			ts2 = timestamp();
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "SA-LCE"
				<< " size=" << sizeInBytes
				<< " time=" << ts2-ts1
				<< endl
				<< (((unsigned int)c) % 146) + 20 // use c so it wont get optimized
				<< endl;
			
			// test sequential access queries on normal data
			c = 0x00;
			ts1 = timestamp();
			for(uint64_t i = 0; i < numberOfQueries*10; i++) {
				c ^= dataRaw[i % numberOfBytes];
			}
			ts2 = timestamp();
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "SA-RAW"
				<< " size=" << sizeInBytes
				<< " time=" << ts2-ts1
				<< endl
				<< (((unsigned int)c) % 146) + 20  // use c so it wont get optimized
				<< endl;
#endif
				delete [] dataLCE.fingerprints;
				delete [] dataLCE.powerTable;
				delete [] dataRaw;
		}
	}


	tsEnd = timestamp();
	cout << "Ended successfully in: " << tsEnd-tsStart << "s"  <<endl;
	return EXIT_SUCCESS;
}
