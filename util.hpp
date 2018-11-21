#ifndef UTIL_INCLUDED   // if x.h hasn't been included yet...
#define UTIL_INCLUDED   //   #define this so the compiler knows it has been included
#include <iostream> 
#include <inttypes.h>
#include <cstdio>
#include <fstream>


using namespace std;

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
    
    /* Calculates and prints basic information about the file */
    static uint64_t calculateSizeOfInputFile(fstream* stream) {
        uint64_t fileStart, fileEnd;
        stream->seekg(0, ios::beg);
        fileStart = stream->tellg();
        stream->seekg(0, ios::end);
        fileEnd = stream->tellg();
        return fileEnd - fileStart;
    }
    
    
    
    /* Checks the state flags of fstream  */
    static void inputErrorHandling(fstream* stream) {
        if(stream->good()) {
            return;
        } else {
            cerr << "Error in fstream\n";
        }
        
        if(!stream->is_open()) {
            cerr << "File not found\n";
            exit(-1);
        }
        
        if(stream->eof()) {
            cout << "End of file reached\n";
            exit(0);
        }
        
        if(stream->bad()) {
            cerr << "Bad fstream\n";
            exit(-1);
        }
        
        if(stream->fail()) {
            cerr << "fsteam failed\n";
            exit(-1);
        } 
    }
    
};
#endif 