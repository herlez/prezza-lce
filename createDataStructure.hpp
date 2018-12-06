#ifndef CREATEDATASTRUCTURE_INCLUDED    
#define CREATEDATASTRUCTURE_INCLUDED

#include <fstream>
#include <inttypes.h>
#include "util.hpp"


class createDataStructure{
	
	
	
public: 
	static void loadFile(std::ifstream* text, uint64_t data[], uint64_t fileSizeInByte) {
		text->seekg(0);
		text->read((char*) data, fileSizeInByte);
	}


	static void calculateFingerprints(uint64_t text[], uint64_t fingerprints[], uint64_t numberOfBlocks, unsigned __int128 prime) {
		
		if(numberOfBlocks != 0) {
			fingerprints[0] = text[0];
		}
		
		for (uint64_t i = 1; i < numberOfBlocks; i++) {
			fingerprints[i] = calculateFingerprint(text[i], fingerprints[i-1], prime);
		}
	}


	
	static void calculateExponents (uint64_t powArray[], unsigned __int128 prime, unsigned int size) {
	
		unsigned __int128 X = 2^64 % prime;
		powArray[0] = X; 
	
		for(unsigned int i = 1; i < size ; i++) {
			X = (X * X) % prime;
			powArray[i] = (uint64_t) X;
		}
	}
	
	static uint64_t getBlock (uint64_t data[], uint64_t index, unsigned __int128 prime) {
		if (index == 0) {
			return data[0];
		} else {
			return calculateBlock(data[index-1], data[index], prime);
		}
		
	} 
	
	static char getChar (uint64_t data[], uint64_t index, unsigned __int128 prime) {
		uint64_t blockNumber = index / 8;
		uint64_t offset = index % 8;
		uint64_t block = getBlock(data, blockNumber, prime);
		return ((char*)block)[offset];
		
		
		
	}


private: 

	static uint64_t calculateFingerprint(uint64_t currentB, uint64_t oldP, unsigned __int128 prime) {
		/*If we had to save the information that the block has 1 as the MSB, our fingerprint got modified. Here we reset the first bit again. */
		if(oldP > 0x8000000000000000ULL) {
			 oldP = oldP - 0x8000000000000000ULL;
		}
		
		/* We need 128 bit to store the prime */
		unsigned __int128 X = oldP;
		X = X << 64;
		X = X + currentB;
		
		#ifdef consoleIO
		std::cout << "last P:                             "; util::printInt64(oldP);
		std::cout << "X after addition b: "                ; util::printInt128(X);
		#endif 
		
		X = X % prime;
		
		#ifdef consoleIO
		std::cout << "X after modulo q:   "; util::printInt128(X);
		#endif
		
		/* Set the first bit again if the block is bigger then the prime */
		if(currentB > prime) {
			X = X + 0x8000000000000000ULL;
		}
		return (uint64_t) X;
		
	}
	
	static uint64_t calculateBlock(uint64_t previousFingerprint, uint64_t currentFingerprint, unsigned __int128 prime) {
		if(previousFingerprint > 0x8000000000000000ULL) {
			 previousFingerprint -= 0x8000000000000000ULL;
		}
		
		unsigned __int128 X = previousFingerprint;
		X = X << 64;
		X = X % prime;
		
		int sBit = 0;
		if(currentFingerprint >= 0x8000000000000000ULL) {
			sBit = 1;
			currentFingerprint -= 0x8000000000000000ULL;
		}
		 
		uint64_t Y = (uint64_t) X;
		if (Y <= currentFingerprint) {
			Y = currentFingerprint - Y;
		} else {
			Y = prime - (Y - currentFingerprint);
		}

		if (currentFingerprint >= 0x8000000000000000ULL) {
			Y += prime;
		}
		return Y + sBit*(uint64_t)prime;
	}

};
#endif
