#ifndef LCEQUERIES_INCLUDED    
#define LCEQUERIES_INCLUDED

#include "util.hpp"
#include "createDataStructure.hpp"


class lceQueries{
	
public:


	static uint64_t lce(lceDataStructure * lceData, uint64_t indexI, uint64_t indexJ) {
		
		uint64_t lce = 0;
		
		for (unsigned int i = 0; i < 8; i++) {
			
			if( indexI + i < lceData.sizeInBytes && 
				indexJ + i < lceData.sizeInBytes &&
				createDataStructure.getChar(lceData, indexI + i) == createDataStructure.getChar(lceData, indexJ + j)) {
					return i;
			}
		}
		
		/* Do exponential search to look for the first 2^k blocks with the same fingerprint */	
		for (unsigned int exponent = 0; ; i++) {
			//check borders
			
			// check
			
			
		}  
		
		/* Do binary search on the rest
			 
			return 9;
		
	}




	static uint64_t getFingerprint(lceDataStructure * lceData, uint64_t from, int exponent) {	
	if (from = 0) {
		return fingerPrintTo(lceData, 2^exponent);
	} else {
		
		__int128 =
		
		
		
		fingerPrintTo(lceData, to) - fingerPrintTo(lceData, from) * lceData.powerTable[exponent];
	}
	return 0;
	}





private:
	
	
	static uint64_t fingerPrintTo(lceDataStructure * lceData, uint64_t to) {
	unsigned __int128 fingerprint;
	
	if(to < 8) {
		fingerprintToI = 0;
	} else {
		fingerprint = lceData->fingerprints[(to/8)-1];
		fingerprint = fingerPrint % 0x8000000000000000ULL;
	}
	
	int padding = (to % 8 * 8);
	fingerprint <<= padding;
	
	fingerprint += ( createDataStructure::getBlock(lceData, to/8)  >> (64 - padding) );
	fingerprint = fingerPrint % lceDataStructure.prime;
	
	return (uint64_t) fingerprint;	
	}
	
}

#endif
