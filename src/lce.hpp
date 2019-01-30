#ifndef PREZZA_LCE_INCLUDED  
#define PREZZA_LCE_INCLUDED

#include "util.hpp"
#include <math.h>
#include <inttypes.h>
#include <fstream>

/* This is the main header for Prezza's LCE data structure.
 * It includes functions to build the data structure and to do LCE queries on it. 
 * Also there are methods to do a naive LCE query on a raw text. */




class lce{
	/* This is the lce data structure.
	 * If stores how big it is, the used prime, all 64-bit long fingerprints and the powers, that help calculating LCE faster. */
public:
	struct lceDataStructure {
		uint64_t numberOfBytes;
		uint64_t numberOfBlocks;
		__int128 prime;
		uint64_t * fingerprints;
		uint64_t * powerTable;
	};
	
	/* Build the data structure 'lceData' from the first 'sizeInBytes' bytes of the input file */
	static void buildLCEDataStructure(lceDataStructure * lceData, std::string filePath, uint64_t sizeInBytes) {
		std::ifstream inputStream (filePath, std::ios::in|std::ios::binary);
		util::inputErrorHandling(&inputStream);
		if(sizeInBytes > util::calculateSizeOfInputFile(&inputStream)) {
			std::cerr << "ERROR: The file is smaller than" << sizeInBytes << "Bytes" << std::endl;
			exit(EXIT_FAILURE);
		}
		buildLCEDataStructure(lceData, &inputStream, sizeInBytes);
	}
	
		
	/* Build the data structure 'lceData' from the input file */
	static void buildLCEDataStructure(lceDataStructure * lceData, std::string filePath) {
		std::ifstream inputStream (filePath, std::ios::in|std::ios::binary);
		util::inputErrorHandling(&inputStream);
		buildLCEDataStructure(lceData, &inputStream, util::calculateSizeOfInputFile(&inputStream));
	}
	
	/* Do a naivelce query. This needs O(n) time. */
	static uint64_t naivelce(char * rawData, uint64_t size, uint64_t indexI, uint64_t indexJ) {
		uint64_t lce = 0;
		if(indexI == indexJ) {
			return (size - indexI);
		} else {			
			while ((indexI < size) && (indexJ < size) && (rawData[indexI] == rawData[indexJ])) {
				indexI++;
				indexJ++;
				lce++;
			}
		}
		return lce;
	}
	
	
	
	/* This performs a fastLCE query on the the LCE data structure in O(log(n) time. */
	static uint64_t fastlce(lceDataStructure * lceData, uint64_t indexI, uint64_t indexJ) {
		
		/* Check if indexes are the same beforehand */
		if (indexI == indexJ) {
			return lceData->numberOfBytes - indexI;
		}
		/* Check single character before comparing blocks 
		 * This can be used to speed up LCE queries with short extensions,
		 *  because linear comparison  is faster */
		for (unsigned int i = 0; i < 0; i++) {
			
			if( indexI + i >= lceData->numberOfBytes || 
				indexJ + i >= lceData->numberOfBytes ||
				getChar(lceData, indexI + i) != getChar(lceData, indexJ + i)) {
					return i;
			}
		}
		
		/* Do exponential search to look for the first 2^k blocks with the same fingerprint */	
		unsigned int exponent = 0;	
		while(  indexI + pow(2, exponent) <= lceData->numberOfBytes && //first check borders
				indexJ + pow(2, exponent) <= lceData->numberOfBytes && 
				(getFingerprint(lceData, indexI, exponent) == getFingerprint(lceData, indexJ, exponent) ) ) { //then check if fingerprints are the same
			
			exponent++;
		}
		/* If exponent == 0 nothing matched, so there is no common extension */
		if(exponent == 0) { return 0;}
		/* If exponent == 1, exactly one character matched, so we can return 1 */
		if(exponent == 1) { return 1;}
		exponent--; // because the current exponent leads to different fingerprints, we lower it by 1. We know that 2^(exp-1) character matched.
		
		
		/* Do binary search from the part on that did not match */
		uint64_t newIndexI = indexI + pow(2, exponent);
		uint64_t newIndexJ = indexJ + pow(2, exponent);
		
		
		while (exponent > 0) {
			if( newIndexI + pow(2, exponent) <= lceData->numberOfBytes && //first check borders
				newIndexJ + pow(2, exponent) <= lceData->numberOfBytes &&
				(getFingerprint(lceData, newIndexI, exponent) == getFingerprint(lceData, newIndexJ, exponent) ) ){ //then check if fingerprints are the same
					newIndexI += pow(2, exponent);
					newIndexJ += pow(2, exponent);
			} else {
				exponent--;
			}
		} 
		/* Check if the last character matches */
		if( getChar(lceData, newIndexI) == getChar(lceData, newIndexJ) ) {
			newIndexI++;
		}
		
		return (newIndexI - indexI);   
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
		uint64_t offset = 7 - (index % 8);
		uint64_t block = getBlock(lceData, blockNumber);
		return ((char*)&block)[offset];
	}
	
	

	
private:

	static uint64_t getFingerprint(lceDataStructure * lceData, uint64_t from, int exponent) {	
		if (from == 0) {
			return fingerprintTo(lceData, pow(2, exponent)-1); // ie if exponent = 3, we want P[0..7];
		} else {
			unsigned __int128 fingerprintToI = fingerprintTo(lceData, from - 1);
			unsigned __int128 fingerprintToJ = fingerprintTo(lceData, from + (pow(2, exponent))-1);

			fingerprintToI *= lceData->powerTable[exponent];
			fingerprintToI %= lceData->prime;

			if (fingerprintToJ >= fingerprintToI) {
				return (uint64_t) (fingerprintToJ - fingerprintToI);
			} else {
				return (uint64_t) (lceData->prime - (fingerprintToI - fingerprintToJ));
			}
		}
	}

	




	static void buildLCEDataStructure(lceDataStructure * lceData, std::ifstream * fileStream, uint64_t sizeInBytes) {
		lceData->prime = util::getLow64BitPrime();
		lceData->numberOfBytes = sizeInBytes;
		util::inputErrorHandling(fileStream);
		
		/* Set size in blocks and calculate number of blocks from it */
		lceData->numberOfBlocks = lceData->numberOfBytes/8;
		if (lceData->numberOfBytes%8 != 0) {
			lceData->numberOfBlocks++;
		}
		lceData->fingerprints = new uint64_t[lceData->numberOfBlocks];
		
		/* Load the text into the right array, overwrite it then with the fingerprints */
		loadFile(fileStream, (char*)lceData->fingerprints, lceData->numberOfBytes);
        
        
        /* Reverse the order of the bytes in every fingerprint. Calculations are easier then */
        char * input = (char*) (lceData->fingerprints);
        for(uint64_t i = 0; i < lceData->numberOfBlocks; i++) {
            uint64_t paquet = *(uint64_t*)"\x1\x0\x0\x0\x0\x0\x0\x0";
            if(paquet == 1){
                //reverse
                char *f=&input[0], *b=&input[7];
                while(f<b){
                    char tmp = *f;
                    *f++ = *b;
                    *b-- = tmp;
                }
            }
            input += 8;
        }  
		/* Here all fingerprints are calculated */
        
        
		lceData->fingerprints[0] = lceData->fingerprints[0];
        for (uint64_t i = 1; i < lceData->numberOfBlocks; i++) {
			lceData->fingerprints[i] = calculateFingerprint(lceData->fingerprints[i], lceData->fingerprints[i-1], lceData->prime);
		}
		
		calculatePowers(lceData);
	}
	
	static uint64_t calculateFingerprint(uint64_t currentBlock, uint64_t previousFingerprint, __int128 prime) {
		
		if(previousFingerprint > 0x8000000000000000ULL) {
			 previousFingerprint -= 0x8000000000000000ULL;
		}
		
		/* We need 128 bit to store the prime */
		unsigned __int128 X = previousFingerprint;
		X = X << 64;
		X = X + currentBlock;	
		X = X % prime;
		
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
	
	/* VALIDIERT */
	static uint64_t fingerprintTo(lceDataStructure * lceData, uint64_t to) {
	unsigned __int128 fingerprint = 0;
	int padding = ((to+1) % 8) * 8; //i.e. for to = 8, you have to left-pad the fingerprint by 8 bits, and right pad the block by 8 bits to the right; for to=1 you only have to right-pad the block by 16 bits;
	

	if(padding == 0) {
		return lceData->fingerprints[to/8];
	}
	if(to < 8) {
		fingerprint = 0;
	} else {
		fingerprint = lceData->fingerprints[(((to+1)/8) - 1)]; //i.e for to=8 you have to take the first fingerprint; for to=7, you dont need the previous fingerprint
		if(fingerprint > 0x8000000000000000ULL) {
			fingerprint -= 0x8000000000000000ULL; //if the helping bit was set, reset it 
		}
		fingerprint = fingerprint << padding;
	} 
	fingerprint = fingerprint + ((getBlock(lceData, (to+1)/8)) >> (64 - padding));
	fingerprint = fingerprint % lceData->prime;
	return (uint64_t) fingerprint;	
	}
	
	/* Here we calculate all 2^b mod 64 for b = 1, 2, 4, 8, 16.. */
	static void calculatePowers(lceDataStructure * lceData){
		unsigned int numberOfLevels = ((int) std::log2(lceData->numberOfBlocks)) + 6; // +1 to round up and +4 because we need bit shifts by 1byte, 2byte, 4byte and 8byte
		lceData->powerTable = new uint64_t[numberOfLevels];
		unsigned __int128 X = 2^8 % lceData->prime;
		X = 256; //first shift is by one byte
		lceData->powerTable[0] = (uint64_t) X;
		
		for (unsigned int i = 1; i < numberOfLevels; i++) {
			X = (X*X) % lceData->prime;
			lceData->powerTable[i] = (uint64_t) X;
		}
	}
	static void loadFile(std::ifstream* text, char data[], uint64_t fileSizeInByte) {
		text->seekg(0);
		text->read(data, fileSizeInByte);
	}
	
};
#endif
