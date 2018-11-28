#ifndef CREATEDATASTRUCTURE_INCLUDED    
#define CREATEDATASTRUCTURE 

#include <fstream>
#include <inttypes.h>
#include "util.hpp"



class createDataStructure{
	
	
	
public: 
	static void loadFile(std::ifstream* text, uint64_t data[], uint64_t fileSizeInByte) {
		text->seekg(0);
		text->read((char*) data, fileSizeInByte);
	}


	static void calculateFingerprints(uint64_t text[], uint64_t fingerprints[], unsigned __int128 prime, uint64_t numberOfBlocks) {
		
		if(numberOfBlocks != 0) {
			fingerprints[0] = text[0];
		}
		
		for (uint64_t i = 1; i < numberOfBlocks; i++) {
			fingerprints[i] = calculateFingerprint(text[i], fingerprints[i-1], prime);
		}
	}		


	static unsigned __int128 randomPrime() {
		
		srand(time(0));
		unsigned const __int128 primes[] = { 0x800000000000001dULL, 0x8000000000000063ULL, 0x800000000000007bULL, 0x8000000000000083ULL, 0x800000000000009bULL };
		unsigned const int numberOfPrimes = 5;
		
		
		unsigned __int128 prime = primes[(rand() % numberOfPrimes)];
		return prime;
	}


private: 

	static uint64_t calculateFingerprint(uint64_t currentB, uint64_t oldP, unsigned __int128 prime) {
	/*If we had to save the information that the block has 1 as the MSB, our fingerprint got modified. Here we reset the first bit again. */
    oldP = oldP & ~0x8000000000000000ULL;
	
	/* We need 128 bit to store the prime */
    unsigned __int128 X = oldP;
    X = X << 64;
    X = X + currentB;
    
    #ifdef consoleIO
    cout << "last P:                             "; util::printInt64(oldP);
    cout << "X after addition b: "                ; util::printInt128(X);
    #endif 
    
    X = X % prime;
    
    #ifdef consoleIO
    cout << "X after modulo q:   "; util::printInt128(X);
    #endif
    
    return (uint64_t) X;
		
	}


};
#endif