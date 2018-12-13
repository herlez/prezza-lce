#include <fstream>
#include <iostream> 
#include <cstdlib>
#include <inttypes.h>
#include "util.hpp"
#include "createDataStructure.hpp"
#include <sys/time.h>
//#include <cmath> 
//#include <ctgmath>

#define consoleIO

using namespace std;

double timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int main(int argc, char *argv[]) {
	/* Check command line arguments */
    if(argc != 3) {
        cout << "Usage: ra FILE_PATH, NUMBER_OF_QUERIES\n" 
        << "Example: ra RKFenglish.s 500000\n";
        return EXIT_FAILURE;
    }
	
	string filePath = argv[1];
	uint64_t  numberOfQueries = atoi(argv[2]);
	
	ifstream inputStream (filePath, ios::in|ios::binary);
	util::inputErrorHandling(&inputStream);
	cout << "Got to here \n";
	
	
	/* Construct data structure */
	uint64_t sizeInBytes = util::calculateSizeOfInputFile(&inputStream);
	uint64_t sizeInBlocks = sizeInBytes/8;
	if ( util::calculateSizeOfInputFile(&inputStream)%8 != 0) {
		sizeInBlocks++; 
	}
	cout << "Size in bytes : "; util::printInt64(sizeInBytes);
	cout << "Size in blocks: "; util::printInt64(sizeInBlocks);

	
	
	char * dataRaw = new char[sizeInBytes];
	createDataStructure::lceDataStructure dataLCE;
	
	createDataStructure::loadFile(&inputStream, dataRaw, sizeInBytes);
	createDataStructure::buildLCEDataStructure(&dataLCE, (uint64_t*) dataRaw, sizeInBlocks);
	cout << "Prime number  : "; util::printInt128(dataLCE.prime);
	
	
	
	
	/* ------------------------------
	 * -Random Access is tested here-
	 * ------------------------------*/
	 
	
	double ts1, ts2; //timestamps
	char c = 0x00; //character to overwrite
	uint64_t n = 10000000; //number of random numbers
	uint64_t * rand = new uint64_t[n]; //array for random numbers
	
	
	/* load 80MB random numbers in RAM */
	ts1 = timestamp();
	srand(time(0));	
	for(uint64_t i = 0; i < n; i++) {
		rand[i] = util::randomIndex(sizeInBytes);
	}
	ts2 = timestamp();
	cout << "Time for generating random indexes was: " << ts2-ts1 << endl;
	
	
	
	
	/* Do random access queries on lce data structure */
	c = 0x00;
	ts1 = timestamp();
	for(uint64_t i = 0; i < numberOfQueries; i++) {
		c |= createDataStructure::getChar(&dataLCE, rand[i % n]);
	}
	ts2 = timestamp();
	cout << c << "| Finished with LCE data structure RA in " << ts2 - ts1 << endl;
	

	
	
	/* Do random access queries on normal data*/
	c = 0x00;
	ts1 = timestamp();
	for(uint64_t i = 0; i < numberOfQueries; i++) {
		c |= dataRaw[rand[i % n]];
	}
	ts2 = timestamp();
	cout << c <<"| Finished with standard data RA in " << ts2 - ts1 << endl;
	

	cout << "Ended successfully" <<endl;
	return EXIT_SUCCESS;
}


