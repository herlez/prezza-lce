#ifndef UTIL_INCLUDED    
#define UTIL_INCLUDED     
#include <iostream> 
#include <inttypes.h>
#include <cstdio>
#include <fstream>


class util{


public:
    
    
    /* Prints the 64-bit long integer in HEX*/
    static void printInt64(uint64_t n) {
            printf("%016lX\n", n);
    }
    
    /* Prints the 128-bit long integer in HEX*/
    static void printInt128(unsigned __int128 n) {
        uint64_t * numberHalf = new uint64_t[2];
        numberHalf = (uint64_t*) &n;
        printf("%016lX%016lX\n", numberHalf[1], numberHalf[0]);
    }
    
    /* Prints information about the data structure*/
    static void printEnd(uint64_t fileSize) {
        std::cout << fileSize << " Bytes processed" << std::endl;
        std::cout << "\n----------------------------------------------------\n";
        std::cout << "-----------------PROGRAM END------------------------\n";
        std::cout << "----------------------------------------------------\n\n";
} 
    
    
    /* Returns the length of the file in bytes */
    static uint64_t calculateSizeOfInputFile(std::fstream* stream) {
        uint64_t fileStart, fileEnd;
        stream->seekg(0, std::ios::beg);
        fileStart = stream->tellg();
        stream->seekg(0, std::ios::end);
        fileEnd = stream->tellg();
        return fileEnd - fileStart;
    }
    
    /* Returns a prime number from a precalculated set */
    static unsigned __int128 getLow64BitPrime() {
		
		srand(time(0));
		unsigned const __int128 primes[] = { 0x800000000000001dULL, 0x8000000000000063ULL, 0x800000000000007bULL, 0x8000000000000083ULL, 0x800000000000009bULL };
		unsigned const int numberOfPrimes = 5;
		
		
		unsigned __int128 prime = primes[(rand() % numberOfPrimes)];
		return prime;
	}
	
	
    /* Returns the length of the file in bytes */
    static uint64_t calculateSizeOfInputFile(std::ifstream* stream) {
        uint64_t fileStart, fileEnd;
        stream->seekg(0, std::ios::beg);
        fileStart = stream->tellg();
        stream->seekg(0, std::ios::end);
        fileEnd = stream->tellg();
        return fileEnd - fileStart;
    }
    
    static uint64_t randomIndex(uint64_t max) {
		uint64_t randIndex = rand();
		randIndex <<= 32;
		randIndex += rand();
	
		randIndex = randIndex % max;
		return randIndex;
	}
    
    
    /* Checks the state flags of fstream  */
    static void inputErrorHandling(std::fstream* stream) {
        if(stream->good()) {
            return;
        } else {
            std::cerr << "Error in fstream\n";
        }
        
        if(!stream->is_open()) {
            std::cerr << "File not found\n";
            exit(-1);
        }
        
        if(stream->eof()) {
            std::cout << "End of file reached\n";
            exit(0);
        }
        
        if(stream->bad()) {
            std::cerr << "Bad fstream\n";
            exit(-1);
        }
        
        if(stream->fail()) {
            std::cerr << "fsteam failed\n";
            exit(-1);
        } 
    }
	
	static void inputErrorHandling(std::ifstream* stream) {
        if(stream->good()) {
            return;
        } else {
            std::cerr << "Error in fstream\n";
        }
        
        if(!stream->is_open()) {
            std::cerr << "File not found\n";
            exit(-1);
        }
        
        if(stream->eof()) {
            std::cout << "End of file reached\n";
            exit(0);
        }
        
        if(stream->bad()) {
            std::cerr << "Bad fstream\n";
            exit(-1);
        }
        
        if(stream->fail()) {
            std::cerr << "fsteam failed\n";
            exit(-1);
        } 
    }
    
};
#endif 
