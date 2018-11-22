#include <iostream> 
#include <fstream>
#include <inttypes.h>
#include <cstdlib> 
#include "util.hpp"
//#define consoleIO /* This prints out every block/fingerprint for debug/analysis */

using namespace std;


const int tau = 64; /* Size of each block in bits */
const int bytesize = 8; /* Size of one character in bits */
const streampos blockSize = tau/bytesize; /* Number of characters per block */


/*We need a 64 bit prime, close to 2^63.
Here we store the 5 smallest primes and choose a random one in our LCE data structure construction*/
unsigned const __int128 primes[] = { 0x800000000000001dULL, 0x8000000000000063ULL, 0x800000000000007bULL, 0x8000000000000083ULL, 0x800000000000009bULL };
unsigned const int numberOfPrimes = 5;
unsigned __int128 q; /* The prime number */


/*Prezza suggests to use a random seed between 0 and q-1. Because we won't run in any collision problems, we use the seed to store our prime number. This way we can reconstruct the text without any additional information*/
uint64_t seed;


uint64_t fileSize; /* The size of the file in byte */
uint64_t numberOfBlocks; /* The size of the file in blocks */


uint64_t B; /*Current block*/
uint64_t P = 0; /*Current fingerprint*/
unsigned __int128 X; /* We need an 128 bit long integer in order to calculate the fingerprint. (P << 64) + B has to be stored for this purpose */


fstream input; /* The fstream, which reads from the file and overwrites the file */



/* Returns a random prime from the array 'primes' */
unsigned __int128 choosePrime() {
    srand(time(0));
    unsigned __int128 prime = primes[(rand() % numberOfPrimes)];
    cout << "Random prime:       "; util::printInt128(prime);
    return prime;
}

/* Here we return the chosen prime number but with the first bit set to 0. So that we can reconstruct the prime with the first block*/
uint64_t chooseSeed() {
    uint64_t tseed = q - 0x8000000000000000ULL;
    cout << "Random seed:                        "; util::printInt64(tseed);
    return tseed;
}

/* Reads a block from the input text and return the integer value of it. */
uint64_t readBlock(long blockNumber) {
    uint64_t block = 0;
    /* The seed is treated as block number 0.*/
    if(blockNumber == 0) {
        return seed;
    } else {
        input.seekg((blockNumber-1)*blockSize); 
        input.read((char*) &block, blockSize);
    }    
    return block;
}

/* Overwrite current block with current fingerprint*/
void writeBlock(long blockNumber) {
    input.seekp((blockNumber-1)*blockSize);
    input.write((char*) &P, blockSize);
}

/* Calculates the new Rabin-Karp fingerprint. The current block and old fingerprint are sufficient for the calculation, because Rabin-Karp is a rolling hash function.*/
uint64_t calculateFingerprint(uint64_t currentB, uint64_t oldP) {
    
    /*If we had to save the information that the block has 1 as the MSB, our fingerprint got modified. Here we reset the first bit again. */
    oldP = oldP & ~0x8000000000000000ULL;
    X = oldP;
    X = X << 64;
    X = X + currentB;
    
    
    #ifdef consoleIO
    cout << "last P:                             "; util::printInt64(oldP);
    cout << "X after addition b: "                ; util::printInt128(X);
    #endif 
    
    X = X % q;
    P = (uint64_t) X;
    
    #ifdef consoleIO
    cout << "X after modulo q:   "; util::printInt128(X);
    cout << "new fingerprint is:                 "; util::printInt64(P);
    #endif
    
    return P;
}

/*Checks whether it is possible to create an in-place LCE datastructure. The condition is that no fingerprint has a 1 as the MSB*/
bool checkForNoCollision() {
    cout << "------------Checking for collisions-----------------\n";
    
    input.seekg(0, ios::beg);
    //unsigned int progress = 1;
    for(unsigned long i = 0; i <= fileSize/blockSize; i++) {
        
        B = readBlock(i);
        P = calculateFingerprint(B, P);
        if (P >= 0x8000000000000000ULL) {
            cout << "Collision detected in Block number " << i << endl;
            return false;
        }
        
        //if(i >= (numberOfBlocks* progress/10)) {
        //    cout <<"\r" << progress << "0%" << flush;
        //   progress++;
        //}
    }
    cout << "\n----------Success. No Collision detected------------\n";
    return true;
}



/* Builds the in-place LCE datastructure. The old text will be overwritten. */
void buildInPlaceLCE() {
    cout << "\n--------Building in-place LCE datastructure---------\n";
    
    /*Here we save out seed in P, because it is the first fingerprint
     We can't overwrite the first block after that, because we need the block to calculate the next fingerprint*/
    B = seed;
    P = calculateFingerprint(B, 0);
    
    /* To track progress */
    //unsigned int progress = 1; 
    
    
    
    for(unsigned long i = 1; i <= fileSize/blockSize; i++) {
        
        B = readBlock(i);
        writeBlock(i);
        P = calculateFingerprint(B, P);
        
        /*Here we have to save the information that B >= q.
        To do this, we set the first bit in P. It should always have been 0 before.*/
        if(B >= q) {
            P |= 0x8000000000000000ULL;
        }
        
        
        ///* Track the progress */
        //if(i >= (numberOfBlocks * progress/10U)) {
        //    cout <<"\r" << progress << "0%" << flush;
        //    progress++;
        //}
    }
    /* Write the last block, which was calculated already */
    writeBlock((fileSize/blockSize)+1);
    cout << "\n--------Successfully build LCE datastructure--------\n";
    cout << "Number of blocks: " << numberOfBlocks << endl;
}

uint64_t* inMemoryBuildInPlaceLCE() {
        
    uint64_t * LCEDataStructure = new uint64_t[numberOfBlocks+1];
    B = seed;
    P = calculateFingerprint(B, P);
        
    for(unsigned long i = 1; i <= fileSize/blockSize; i++) {
        
        B = readBlock(i);
        LCEDataStructure[i-1] = P;
        P = calculateFingerprint(B, P);
        
        /*Here we have to save the information that B >= q.
        To do this, we set the first bit in P. It should always have been 0 before.*/
        if(B >= q) {
            P |= 0x8000000000000000ULL;
        }
    }
    /* Write the last block, which was calculated already */
    LCEDataStructure[numberOfBlocks] = P;
    cout << "\n----Successfully build LCE datastructure in Memory---\n";
    cout << "Number of blocks: " << numberOfBlocks << endl;
    return LCEDataStructure;
}    




/* This is the back up plan, in case building the in-place data structure does not work. */ 
void safePlan() {
    cout << "dadidadida\n";
}


int main(int argc, char *argv[]) {
    
    /* Initialize the fstream */
    if(argc != 2) {
        cout << "Usage: createDataStructure FILE_PATH\n" 
        << "Example: createDataStructure RKFenglish.s\n";
        return EXIT_FAILURE;
    }
    
    input.open(argv[1], ios::in|ios::out|ios::binary);
    util::inputErrorHandling(&input);
    
    
    
    cout << "\n----------------------------------------------------\n";
    cout << "-----------------PROGRAM START----------------------\n";
    cout << "----------------------------------------------------\n\n";
    
    fileSize = util::calculateSizeOfInputFile(&input);
    numberOfBlocks = fileSize/blockSize;
    cout << "File size:          " << fileSize << " byte\n";
    cout << "File size:          " << numberOfBlocks      << " blocks\n";
    
    
    /* Choose a random prime and a random seed */
    q = choosePrime();
    seed = chooseSeed();
    
    
    /* Build the data structure */
    if(checkForNoCollision()) {
        buildInPlaceLCE();
    } else {
        safePlan();
    }
     
    
    /* Print additional information and exit */
    util::printEnd(fileSize); 
    input.close();
    return EXIT_SUCCESS;
    
}