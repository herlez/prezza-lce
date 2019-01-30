#include <inttypes.h>
#include "lce.hpp"
#include "util.hpp"

/* Define which tests should be done */
#define testChar
#define testBlock
#define testLCE


/* This programs checks if the extraction of blocks and characters is working correctly.
 * It also checks if LCE queries return the correct value */





/* Enter the file you want to test on here */
const std::string TESTFILE = "../text/english"; 



using namespace std;
int main(int argc, char *argv[]) {
	
	
    /* Construct LCE data structure */
    cout << "Constructing LCE data stucture" << endl;
	lce::lceDataStructure dataLCE;
	lce::buildLCEDataStructure(&dataLCE, TESTFILE);
	
	/* Load raw Text */
	cout << "Loading raw text" << endl;
	ifstream input(TESTFILE, ios::in|ios::binary);
	util::inputErrorHandling(&input);
	uint64_t dataSize = util::calculateSizeOfInputFile(&input);
    char * dataRaw = new char[dataSize];
    input.seekg(0);
    input.read(dataRaw, dataSize);
	
	cout << "Size of data: " << dataLCE.numberOfBytes << " Bytes" << endl;
	cout << "Size of data: " << dataLCE.numberOfBlocks << " Blocks" << endl;
	
	
    
#ifdef testChar
	/* Test if every character can be extracted correctly */
	cout << "Testing if every character is extracted correctly" << endl;
	for(uint64_t i = 0; i < dataLCE.numberOfBytes; i++) {
		
		if(dataRaw[i] != lce::getChar(&dataLCE, i)) {
			cout << "Wrong Char: Error at position " << i << endl;
		}
	}
#endif



#ifdef testBlock
	uint64_t paquet = *(uint64_t*)"\x1\x0\x0\x0\x0\x0\x0\x0"; /* To check if small endian is used */
	/* Test if every block can be extracted correctly */
	cout << "Testing if every block is extracted correctly" << endl;
	uint64_t * blockRaw = (uint64_t*) dataRaw;

        
	for(uint64_t i = 0; i < dataLCE.numberOfBlocks; i++) {
		
		uint64_t block = blockRaw[i];
		
		/* If small endian, reverse the character bytes, so it matches the endianess of uint64_t */
		if(paquet == 1){
		//reverse
			char * cArray = (char*) &block;
			char *f=&cArray[0], *b=&cArray[7];
			while(f<b){
				char tmp = *f;
				*f++ = *b;
				*b-- = tmp;
			}
		}
		
		
		if(i == dataLCE.numberOfBlocks - 1) {
			if(block != lce::getBlock(&dataLCE, i)) {
				cout << "Wrong Block: Error at position " << i << endl;
				cout << "Raw Block: "; util::printInt64(blockRaw[i] << ((dataLCE.numberOfBytes%8)*8) >> ((dataLCE.numberOfBytes%8)*8));
				cout << "Block Fingerprint: "; util::printInt64(lce::getBlock(&dataLCE, i));
			}
		}
	}
#endif
	
	
#ifdef testLCE
	/* Test if LCE Queries are correct */
	cout << "Testing if LCE queries are correct" << endl;
	uint64_t lceRaw = 0;
	uint64_t lceFast = 0;
	
	for(uint64_t i = 0; i < 2000; i++) {
		for(uint64_t j = 0; j < 2000; j++) {
			lceRaw = lce::naivelce(dataRaw, dataSize, i, j);
			lceFast = lce::fastlce(&dataLCE, i, j);
			
			if(lceRaw != lceFast) {
				cout << "Wrong LCE: Error at position " << i << " and " << j << endl;
				cout << "lceRaw:  " << lceRaw << endl;
				cout << "lceFast: " << lceFast << endl;
			}
		}
	}
#endif


	
	exit(EXIT_SUCCESS);
}
