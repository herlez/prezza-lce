#include <fstream>
#include <iostream> 
#include <cstdlib>
#include <inttypes.h>
#include "util.hpp"

//#define consoleIO
using namespace std;


const int blockSize = 8;


fstream input;
fstream output;
uint64_t fileSize;
uint64_t numberOfBlocks;


uint64_t oldP = 0; 
uint64_t P = 0; 
uint64_t B = 0;

/* The prime used for data structure construction */
unsigned __int128 q = 0;
unsigned __int128 X = 0;




void printSizeOfInputFile() {
    uint64_t fileStart, fileEnd;
    fileStart = input.tellg();
    input.seekg(0, ios::end);
    fileEnd = input.tellg();
    fileSize = fileEnd - fileStart;
    
    numberOfBlocks = fileSize/blockSize;
    cout << "File size:          " << fileSize << " byte\n";
    cout << "File size:          " << numberOfBlocks      << " blocks\n";
}

uint64_t readBlock(int blockNumber) {
    uint64_t block;
    input.seekg(blockNumber*blockSize); 
    input.read((char*) &block, blockSize);
    util::inputErrorHandling(&input);
    
    
    return block;
    
}

void writeBlock(int blockNumber) {
    output.seekp(blockNumber*blockSize);
    output.write((char*) &B, blockSize);
    util::inputErrorHandling(&output);
}

uint64_t calculateBlock( uint64_t fingerprint, uint64_t oldFingerprint) {    
    
    X = (oldFingerprint & ~0x8000000000000000ULL);
    X = X << 64;
    X = X % q;
    uint64_t Y = (uint64_t) X;
    if (Y <= fingerprint) {
        Y = fingerprint - Y;
    } else {
        Y = q - (Y - fingerprint);
    }

    if (fingerprint >= 0x8000000000000000ULL) {
        Y += q;
    }
    
#ifdef consoleIO
    cout << "old P:      "; util::printInt64(oldFingerprint);
    cout << "new P:                      " ; util::printInt64(fingerprint);
    cout << "Y:          "; util::printInt64(Y);
#endif
    
    return Y;
}

void reconstructDataStructure() {
    
    /*The first block is the prime which was used for construction. 
     We need it to reconstruct the text.*/
    oldP = readBlock(0);
    q = (unsigned __int128)(readBlock(0)+ 0x8000000000000000ULL);
    cout << "The fingerprint used for construction was: "; util::printInt128(q);
    
    
    for (unsigned int i = 1; i < numberOfBlocks; i++) {
        P = readBlock(i);
        B = calculateBlock(P, oldP);

        writeBlock(i-1);
        
        oldP = P & ~0x8000000000000000ULL;
    }
    
}


int main(int argc, char *argv[]) {
    
    /* Check command line arguments */
    if(argc != 2) {
        cout << "Usage: reconstructDataStructure FILE_PATH\n" 
        << "Example: reconstructDataStructure RKFenglish.s\n";
        return EXIT_FAILURE;
    }
    
    
    /* Initialize all streams. Output is saved in a new file named file_name_RECONSTRUCTED */
    string filePath = argv[1];
    output.open((filePath + "_RECONSTRUCTED").c_str() , ios::out|ios::trunc|ios::binary);
    input.open(filePath.c_str(), ios::in|ios::binary);
    util::inputErrorHandling(&input);
    /* Calculate and print the size of the file in byte and blocks */
    fileSize = util::calculateSizeOfInputFile(&input);
    numberOfBlocks = fileSize/blockSize;
    cout << "File size:          " << fileSize << " byte\n";
    cout << "File size:          " << numberOfBlocks      << " blocks\n";
    
    
    /* Reconstruct the text file */
    reconstructDataStructure();

    
    cout << "Successfully reconstructed data structure\n";
    return EXIT_SUCCESS;
}