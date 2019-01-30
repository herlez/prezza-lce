#include <fstream>
#include <iostream> 
#include <cstdlib>
#include <inttypes.h>
#include <sys/time.h>


#include "util.hpp"
#include "lce.hpp"


//#define consoleIO

using namespace std;

double timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int main(int argc, char *argv[]) {
	
	ofstream log("../test/time.txt", ios::out|ios::trunc);
	
	string fileNames[] {"english", "proteins", "dna", "a"};
	string files[] = {"../text/english", "../text/proteins", "../text/dna", "../text/a"};
	bool testFinished[] = {false, false, false, false};
	
	 
	double ts1, ts2, tsStart, tsEnd; //timestamps
	lce::lceDataStructure dataLCE;
	/* Beforehand generate random numbers */
	uint64_t n = 10000000; //number of random numbers; 10000000 = 80MB
	uint64_t * rand = new uint64_t[n]; //array for random numbers
	srand(0);
	

	uint64_t numberOfQueries = 10000000;
	
	tsStart = timestamp();
	/* MAIN TEST */
	for(unsigned int fileNumber = 0; fileNumber < 4; fileNumber++) {		
		string filePath = files[fileNumber];
		ifstream inputStream (filePath, ios::in|ios::binary);
		util::inputErrorHandling(&inputStream);
		
		/* Test with up to 8GB of data */
		for (uint64_t numberOfBytes = 1<<8; numberOfBytes <= 10000000000ULL && !testFinished[fileNumber]; numberOfBytes = numberOfBytes*2) {
			if (numberOfBytes > util::calculateSizeOfInputFile(&inputStream)) {
				numberOfBytes = util::calculateSizeOfInputFile(&inputStream);
				testFinished[fileNumber] = true;
				
			}
			//load random number which are used as indexes
			for(uint64_t i = 0; i < n; i++) {
				rand[i] = util::randomIndex(numberOfBytes);
			}
			
			//load Text
			ts1 = timestamp();
			char * dataRaw = new char[numberOfBytes];
			inputStream.seekg(0);
			inputStream.read(dataRaw, numberOfBytes);
			ts2 = timestamp();
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "loadFile"
				<< " size=" << numberOfBytes
				<< " time=" << ts2-ts1
				<< endl;
			

			//build data structure
			ts1 = timestamp();
			
			lce::buildLCEDataStructure(&dataLCE, filePath, numberOfBytes);
			ts2 = timestamp();
			log << "RESULT"
				<< " text=" << fileNames[fileNumber]
				<< " algo=" << "buildDataStructure"
				<< " size=" << numberOfBytes
				<< " time=" << ts2-ts1
				<< endl;
			
			
			
			
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
				<< " size=" << numberOfBytes
				<< " time=" << ts2-ts1
				<< endl 
				<< lceV //use lce so it does not get optimized
				<< endl;
			
			
			
			
			
			//lce on normal data
			lceV = 0;
			ts1 = timestamp();
			// NaiveLCE needs O(n) time, so we wont do it for big 'a' files:
			if(fileNumber != 3 || numberOfBytes <= pow(2, 13)) {
				for(uint64_t i = 0; i < numberOfQueries; i++) {
					uint64_t indexI = rand[i%n];
					uint64_t indexJ = rand[(i+1)%n];
					
					if(indexI == indexJ) {
						lceV += (numberOfBytes - indexI);
					} else {
						
						while (indexI < numberOfBytes && indexJ < numberOfBytes && dataRaw[indexI] == dataRaw[indexJ]) {
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
					<< " size=" << numberOfBytes
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
				<< " size=" << numberOfBytes
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
				<< " size=" << numberOfBytes
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
				<< " size=" << numberOfBytes
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
				<< " size=" << numberOfBytes
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
#endif

	tsEnd = timestamp();
	cout << "Ended successfully in: " << tsEnd-tsStart << "s"  <<endl;
	return EXIT_SUCCESS;
}
