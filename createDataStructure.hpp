#ifndef CREATEDATASTRUCTURE_INCLUDED    
#define CREATEDATASTRUCTURE_INCLUDED

#include <inttypes.h>
#include <cmath>
#include "util.hpp"

class createDataStructure{
public:
	struct lceDataStructure {
		uint64_t numberOfBytes;
		uint64_t numberOfBlocks;
		__int128 prime;
		uint64_t * fingerprints;
		uint64_t * powerTable;
	} ;



	/* Builds the LCE data structure from text and stores it in lceData */
	static void buildLCEDataStructure(lceDataStructure * lceData, uint64_t text[], uint64_t sizeInBytes) {
			
		unsigned __int128 prime = util::getLow64BitPrime();
		
		lceData->prime = prime;
		lceData->numberOfBytes = sizeInBytes;
		
		/* Set size in Blocks and calculate number of blocks from it */
		lceData->sizeInBlocks = sizeInBytes/8;
		if (lceData->sizeInBytes%8 != 0) {
			lceData->sizeInBlocks++; 
		}
		
		/* Here all fingerprints are calculated */
		lceData->fingerprints = new uint64_t[numberOfBlocks];
		lceData->fingerprints[0] = text[0];
		for (uint64_t i = 1; i < numberOfBlocks; i++) {
			lceData->fingerprints[i] = calculateFingerprint(text[i], lceData->fingerprints[i-1], lceData->prime);
		}
		
		calculatePowers(lceData);
	}
	
	
	static void loadFile(std::ifstream* text, char data[], uint64_t fileSizeInByte) {
		text->seekg(0);
		text->read(data, fileSizeInByte);
	}



	static uint64_t getBlock (lceDataStructure * lceData, uint64_t index) {
		if (index == 0) {
			return lceData->fingerprints[0];
		} else {
			return calculateBlock(lceData->fingerprints[index-1], lceData->fingerprints[index], lceData->prime);
		}
		
	} 
	
	static char getChar (lceDataStructure * lceData, uint64_t index) {
		uint64_t blockNumber = index / 8;
		uint64_t offset = index % 8;
		uint64_t block = getBlock(lceData, blockNumber);
		return ((char*)&block)[offset];
	}
	
	


private:
	static uint64_t calculateFingerprint(uint64_t currentBlock, uint64_t previousFingerprint, __int128 prime) {
		
		if(previousFingerprint > 0x8000000000000000ULL) {
			 previousFingerprint -= 0x8000000000000000ULL;
		}
		
		/* We need 128 bit to store the prime */
		unsigned __int128 X = previousFingerprint;
		X = X << 64;
		X = X + currentBlock;
		
		#ifdef consoleIO
		std::cout << "last P:                             "; util::printInt64(previousFingerprint);
		std::cout << "X after addition b: "                ; util::printInt128(X);
		#endif 
		
		X = X % prime;
		
		#ifdef consoleIO
		std::cout << "X after modulo q:   "; util::printInt128(X);
		#endif
		
		/* Set the first bit again if the block is bigger then the prime */
		if(currentBlock > prime) {
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
	
	/* Here we calculate all 2^b mod 64 for b = 1, 2, 4, 8, 16.. */
	static void calculatePowers(lceDataStructure * lceData){
		unsigned int numberOfLevels = ((int) std::log2(lceData->numberOfBlocks)) + 1;
		lceData->powerTable = new uint64_t[numberOfLevels];
		unsigned __int128 X = 2^64 % lceData->prime;
		lceData->powerTable[0] = (uint64_t) X;
		
		for (unsigned int i = 1; i < numberOfLevels; i++) {
			X = (X^2) % lceData->prime;
			lceData->powerTable[i] = (uint64_t) X;
		}
	}

};
#endif
