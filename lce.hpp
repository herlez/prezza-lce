#ifndef LCEQUERIES_INCLUDED    
#define LCEQUERIES_INCLUDED

#include "util.hpp"
#include "createDataStructure.hpp"


class lceQueries{
	
public:


	static uint64_t getFingerprint(lceDataStructure * lceData, uint64_t from, int factor) {
	
	if (from = 0) {
		return fingerPrintTo(lceData, to);
	} else {
		return fingerPrintTo(lceData, to) - fingerPrintTo(lceData, from) * powTable[pow];
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
	fingerprint <<= 2^padding;
	fingerprint += ( createDataStructure::getBlock(lceData, uint64_t) >> (64 - padding) );
	fingerprint = fingerPrint % lceDataStructure.prime;
	
	
	return (uint64_t) fingerprint;	
	}
	
}

#endif
