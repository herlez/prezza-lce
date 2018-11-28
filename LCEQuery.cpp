#include <fstream>
#include <cstdlib>
#include <iostream>
#include "util.hpp"
#include "createDataStructure.hpp"
#include <cstdlib> 
#define consoleIO
using namespace std;




ifstream input;
uint64_t * T;
uint64_t * P;


unsigned __int128 q; /* The prime number */


/* size of input file in byte*/
uint64_t fileSizeInByte; 
uint64_t fileSizeInBlocks;





uint64_t phiToI(uint64_t P[], unsigned__int128 q, uint64_t i, ) {
	uint64_t fingerprintToI = 0;
	X = P[i/8];
	X <<= (i mod 8)*8);
	X += getBlock(P, i) >> 64 - ((i mod 8) * 8);
    
	
	
    return 0;
}


uint64_t phiIToJ(uint64_t i, uint64_t j) {
    
    return 0;
}





uint64_t naiveLCE(unsigned char text[],  uint64_t fileSize, uint64_t index1, uint64_t index2) {
    
	
	if (index1 == index2) {
		cout << "LCE " << fileSize-index1 << '\n';
		return fileSize-index1;
	}
		
	
	int lceResult = 0;
	
	cout << "File size: " << fileSize << '\n';
	cout << index1 << "--" << text[index1] << '\n';
	cout << index2 << "--" << text[index2] << '\n';
	
	while ((index1 < fileSize) && (index2 < fileSize) && (text[index1] == text[index2])) {

		cout << index1 << "--" << text[index1] << '\n';
		cout << index2 << "--" << text[index2] << '\n';
		lceResult++;
		index1++; 
		index2++; 
	}
	cout << "LCE: " << lceResult << '\n';
	return lceResult;

}


long prezzaLCE(uint64_t fingerprints[], uint64_t index1,uint64_t index2) {
    return 0;
}



uint64_t randomIndex() {
    uint64_t randIndex = rand();
    randIndex <<= 32;
    randIndex += rand();
	
	randIndex = randIndex % fileSizeInByte;
    return randIndex;
}

	
	



int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: LCEQuery FILE_PATH NUMBER_OF_RANDOM_QUERIES\n"
        << "Example: LCEQuery RKFenglish.s 5000\n";
        return EXIT_FAILURE;
    }
    uint64_t numberOfQueries = atoi(argv[2]);
    cout << "Number of random LCE queries: " << numberOfQueries << '\n';
	
	
	input.open(argv[1], ios::in|ios::binary);
	util::inputErrorHandling(&input);
	
    
    fileSizeInByte = util::calculateSizeOfInputFile(&input);
	fileSizeInBlocks = fileSizeInByte / 8;
	cout << "File size:          " << fileSizeInByte << " byte\n";
    cout << "File size:          " << fileSizeInBlocks      << " blocks\n";
        
	T = new uint64_t[fileSizeInBlocks];
	P = new uint64_t[fileSizeInBlocks];
	q = createDataStructure::randomPrime();
	cout << "Random prime:       "; util::printInt128(q);

	/* We load the file in T */
	createDataStructure::loadFile(&input, T, fileSizeInByte);
	cout << "Loaded file successfully..\n";
	/* To create the data structure, we calculate all fingerprints */
	createDataStructure::calculateFingerprints(T, P, q, fileSizeInBlocks);
	cout << "Builded LCE datastructure correctly\n";
	cout << "Starting with random LCE queries\n";
	
	srand(time(0));
    uint64_t randIndex1, randIndex2;
    for (unsigned int i = 0; i < numberOfQueries; i++) {
        randIndex1 = randomIndex();
        randIndex2 = randomIndex();
        naiveLCE((unsigned char*)T ,fileSizeInByte, randIndex1, randIndex2);
    }
    naiveLCE((unsigned char*) T, fileSizeInByte, 5, 5);
    
	/*
    for (unsigned int i = 0; i < numberOfQueries; i++) {
        randIndex1 = randomIndex();
        randIndex2 = randomIndex();
        prezzaLCE( P, randIndex1, randIndex2);
    }
	*/
    
    
    
    return EXIT_SUCCESS;
}