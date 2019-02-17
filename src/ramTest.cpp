#include <fstream>
#include <iostream> 
#include <cstdlib>
#include <inttypes.h>
#include <sys/time.h>


#include "util.hpp"
#include "lce.hpp"
#include <chrono>
#include <thread>

using namespace std;


int main(int argc, char *argv[]) {

	
	const string fileNames[] {"english", "a", "proteins", "dna"};
	const string files[] = {"../text/english", "../text/a", "../text/proteins", "../text/dna"};
	
	 
	double tsStart, tsEnd; //timestamps
	lce::lceDataStructure dataLCE;

		
		for(unsigned int fileNumber = 0; fileNumber < 4; fileNumber++) {		
			
			
			string filePath = files[fileNumber];
			ifstream inputStream (filePath, ios::in|ios::binary);
			util::inputErrorHandling(&inputStream);
		
			uint64_t sizeInBytes = util::calculateSizeOfInputFile(&inputStream);
			cout << sizeInBytes << endl;
			lce::buildLCEDataStructure(&dataLCE, filePath, sizeInBytes);
		
			uint64_t lceFast = 0;
	
			for(uint64_t i = 0; i < 2000; i++) {
				for(uint64_t j = 0; j < 2000; j++) {
					lceFast += lce::fastlce(&dataLCE, i, j);
				}
			}
			cout << lceFast << endl;
		
		
		
			
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
				delete [] dataLCE.fingerprints;
				delete [] dataLCE.powerTable;
		}
	
	return EXIT_SUCCESS;
}
