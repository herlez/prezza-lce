#include <fstream>
#include <iostream> 
#include <cstdlib>
#include <inttypes.h>
#include "util.hpp"
#include "createDataStructure.hpp"

#define consoleIO

using namespace std;


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
	
	
	
	/* Construct data structure */
	uint64_t sizeInBytes = util::calculateSizeOfInputFile(&inputStream);
	uint64_t sizeInBlocks = sizeInBytes/8;
	if ( util::calculateSizeOfInputFile(&inputStream)%8 != 0) {
		sizeInBlocks++; 
	}
	cout << "Size in bytes : "; util::printInt64(sizeInBytes);
	cout << "Size in blocks: "; util::printInt64(sizeInBlocks);
	
	uint64_t * dataRaw = new uint64_t[sizeInBlocks];
	uint64_t * dataStructure = new uint64_t[sizeInBlocks];
	
	createDataStructure::loadFile(&inputStream, dataRaw, sizeInBytes);
	unsigned __int128 prime = util::randomPrime();
	cout << "Random prime: "; util::printInt128(prime);
	createDataStructure::calculateFingerprints(dataRaw, dataStructure, sizeInBlocks, prime);
	
	/* Do queries */
	
	srand(time(0));
	uint64_t randIndex;
	for(uint64_t i = 1; i < numberOfQueries; i++) {
		randIndex = util::randomIndex(sizeInBlocks);
		
		
		if(dataRaw[randIndex] != createDataStructure::getBlock(dataStructure, randIndex, prime)) {
			cout << "Index    :"; util::printInt64(randIndex);
			cout << "Raw      :"; util::printInt64(dataRaw[randIndex]);
			cout << "Structure:"; util::printInt64(createDataStructure::getBlock(dataStructure, randIndex, prime));
			cout << "currentP :"; util::printInt64(dataStructure[i]); 
			if(i > 0) { 
			cout << "oldP     :"; util::printInt64(dataStructure[i-1]);
			}
		} 
	}
	
	
	cout << "Ended successfully" << endl;
	return EXIT_SUCCESS;
}
