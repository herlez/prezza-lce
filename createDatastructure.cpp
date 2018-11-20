#include <iostream> 
#include <fstream>
#include <inttypes.h>
#include <cstdlib> 
//#define consoleIO
//#define assert

using namespace std;

/* Size of each block in bits */
const int tau = 64;
/* Size of one character in bits */
const int bytesize = 8;
/* Number of characters per block */
const streampos blockSize = tau/bytesize;


/*We need a 64 bit prime, close to 2^63.
Here we store the 5 smallest primes and choose a random one in our LCE data structure construction*/
unsigned const __int128 primes[] = { 0x800000000000001dULL, 0x8000000000000063ULL, 0x800000000000007bULL, 0x8000000000000083ULL, 0x800000000000009bULL };
unsigned const int numberOfPrimes = 5;
unsigned __int128 q;


/*The random seed. It is supposed to be between 0 and q*/
uint64_t seed;



/*Counts the number of blocks, where 1 is the most significant bit. This information has to be saved */
long blocksWithSetMSB = 0;
/*Counts the number of fingerprints, where 1 is the most significant bit. Idealy schould be zero.*/
long fingerprintsWithSetMSB = 0;
/*Counts the number of blocks processed. */
long numberOfBlocks = 0;



char * iMemblock = new char[tau/bytesize]; /*buffer for current block*/
char * oMemblock = new char[tau/bytesize]; /*buffer for one fingerprint that gets printed*/
/*TODO:DELETE*/char * tMemblock = new char[tau/bytesize]; /*temporary buffer for a fingerprint, because the endianess has to be reversed..*/

uint64_t B = 0; /*current Block*/
uint64_t P = 0; /*current Fingerprint*/
unsigned __int128 X = 0; /* (P << 64) + B has to be stored in order to calculate the fingerprint, */


fstream input; 
uint64_t fileSize;

/* Prints the 64-bit long integer in HEX*/
void printInt64(uint64_t n) {
    printf("%016lX\n", n);
}

/* Prints the 128-bit long integer in HEX*/
void printInt128(unsigned __int128 n) {
    uint64_t * numberHalf = new uint64_t[2];
    numberHalf = (uint64_t*) &n;
    printf("%016lX%016lX\n", numberHalf[1], numberHalf[0]);
}

/* Prints information about the data structure */
void printEnd() {
    cout << numberOfBlocks*blockSize << " Bytes processed" << endl;
    cout << "\n----------------------------------------------------\n";
    cout << "-----------------PROGRAM END------------------------\n";
    cout << "----------------------------------------------------\n\n";
} 

/* Checks the state flags of fstream  */
void inputErrorHandling() {
    if(input.good()) {
        return;
    } else {
        cerr << "Error in fstream\n";
    }
    
    if(!input.is_open()) {
        cerr << "File not found\n";
        exit(-1);
    }
    
    if(input.eof()) {
        cout << "End of file reached\n";
        printEnd();
        exit(0);
    }
    
    if(input.bad()) {
        cerr << "Bad fstream\n";
        exit(-1);
    }
    
    if(input.fail()) {
        cerr << "fsteam failed\n";
        exit(-1);
    } 
}

/* Calculates and prints basic information about the file */
void printSizeOfInputFile() {
    uint64_t fileStart, fileEnd;
    fileStart = input.tellg();
    input.seekg(0, ios::end);
    fileEnd = input.tellg();
    fileSize = fileEnd - fileStart;
    
    numberOfBlocks = fileSize/blockSize;
    inputErrorHandling();
    cout << "File size:          " << fileSize << " byte\n";
    cout << "File size:          " << numberOfBlocks      << " blocks\n";
}




/* Returns a random prime from the array 'primes' */
unsigned __int128 choosePrime() {
    srand(time(0));
    //unsigned __int128 prime = primes[0];
    unsigned __int128 prime = primes[(rand() % numberOfPrimes)];
    cout << "Random prime:       "; printInt128(prime);
    return prime;
}

/* Chooses a random seed between 0 and q-1 */
uint64_t chooseSeed() {
    uint64_t tseed = q;
    srand(time(0));
    do {
        tseed = rand();
        tseed <<= 32;
        tseed += rand();
    } while (tseed >= q);
    
    //tseed=1;
    cout << "Random seed:                        "; printf("%016lX\n", tseed);
    return tseed;
}

/* Reads a block from the input text and return the integer value of it. */
uint64_t readBlock(long blockNumber) {
    //inputErrorHandling();
    uint64_t block = 0;
    if(blockNumber == 0) {
        return seed;
    } else {
        input.seekg((blockNumber-1)*blockSize); 
        input.read(iMemblock, blockSize);
    }
    #ifdef consoleIO
    /* Print Text as chars*/
    cout << "T[" << blockNumber << "] =                             ";
    
    for(unsigned int j = 0; j < blockSize; j++) {
        if (iMemblock[j] == '\n') {
            cout << ' ';
        } else {
            cout << iMemblock[j];
        }
        cout << ' ';
    }
    cout << "\n";
    #endif
    
    
    
    for(unsigned int j = 0; j < blockSize; j++) {
        block <<= 8;
        block += (unsigned char) iMemblock[j];
    }
    
    #ifdef consoleIO
    cout << "B[" << blockNumber << "] =                             ";
    printInt64(block);
    #endif
    
    return block;
}
/* Overwrite current block with current fingerprint*/
void writeBlock(long blockNumber) {
    //inputErrorHandling();
    input.seekp((blockNumber-1)*blockSize);
    input.write((char*) &P, blockSize);
}

/* Calculates the new Rabin-Karp fingerprint. The current block and old fingerprint are sufficient for the calculation, because Rabin-Karp is a rolling hash function.*/
uint64_t calculateFingerprint(uint64_t currentB, uint64_t oldP) {
    
    /*If we had to save the information that the block has 1 as the MSB, our fingerprint got modified. Here we reset the first bit again */
    oldP = oldP & ~0x8000000000000000ULL;
    X = oldP;
    X = X << 64;
    X = X + currentB;
    
    
    #ifdef consoleIO
    cout << "last P:                             "; printInt64(oldP);
    cout << "X after addition b: "                ; printInt128(X);
    #endif 
    
    X = X % q;
    P = (uint64_t) X;
    
    #ifdef consoleIO
    cout << "X after modulo q:   "; printInt128(X);
    cout << "new fingerprint is:                 "; printInt64(P);
    #endif
    
    #ifdef assert
    if(P >= q) {
        cout << "Something is really really wrong. A fingerprint is greater than q." << endl;
        exit(EXIT_FAILURE);
    }
    #endif
    
    return P;
}

/*Checks whether it is possible to create an in-place LCE datastructure. The condition is that no fingerprint has a 1 as the MSB*/
bool checkForNoCollision() {
    cout << "------------Checking for collisions-----------------\n";
    
    input.seekg(0, ios::beg);
    unsigned int progress = 1;
    for(unsigned long i = 0; i <= fileSize/blockSize; i++) {
    //for(unsigned long i = 0; i <= 10; i++) {
        
        B = readBlock(i);
        P = calculateFingerprint(B, P);
        if (P >= 0x8000000000000000ULL) {
            cout << "Collision detected in Block number " << i << endl;
            return false;
        }
        
        //if(i >= (fileSize/blockSize* progress/10)) {
        //    cout <<"\r" << progress << "0%" << flush;
        //   progress++;
        //}
    }
    cout << "\n----------Success. No Collision detected------------\n";
    return true;
}


/* Builds the in-place LCE datastructure. The old text will be overwritten*/
void buildInPlaceLCE() {
    cout << "\n--------Building in-place LCE datastructure---------\n";
    
    //B = seed;
    P = seed;
    
    unsigned int progress = 1; 
    for(unsigned long i = 1; i <= fileSize/blockSize; i++) {
        
        B = readBlock(i);
        
        writeBlock(i);

        P = calculateFingerprint(B, P);
        /*Here we have to save the information that B >= q.
        To do this, we set the first bit in P. It should always have been 0 before.*/
        if(B >= q) {
            P |= 0x8000000000000000ULL;
            blocksWithSetMSB++;
        }
        
        
        /* Track the progress */
        if(i >= (fileSize/blockSize* progress/10)) {
            cout <<"\r" << progress << "0%" << flush;
            progress++;
        }
    }
    writeBlock(fileSize/blockSize);
    cout << "\n--------Successfully build LCE datastructure--------\n";
    cout << "Number of blocks above q: " << blocksWithSetMSB << "/" << numberOfBlocks << endl;
}

/* This is the back up plan, in case building the in-place data structure does not work. */ 
void safePlan() {
    cout << "dadidadida\n";
}



int main(int argc, char *argv[]) {
    
    
    /* Initialize the fstream */
    if(argc != 2) {
        cout << "Usage: createDatastructure FILE_PATH\n" 
        << "Example: createDatastructure RKFenglish.s\n";
        return EXIT_FAILURE;
    }
    
    input.open(argv[1], ios::in|ios::out|ios::binary);
    inputErrorHandling();
    
    
    
    cout << "\n----------------------------------------------------\n";
    cout << "-----------------PROGRAM START----------------------\n";
    cout << "----------------------------------------------------\n\n";
    
    printSizeOfInputFile();
    
    
    /* Choose a random prime and a random seed */
    q = choosePrime();
    seed = chooseSeed();
    
    
    /* Build the data structure */
    if(checkForNoCollision()) {
        cout << "Random prime:       "; printInt128(q);
        cout << "Random seed:                        "; printInt64(seed);
        buildInPlaceLCE();
    } else {
        safePlan();
    }
     
    
    /* Print additional information and exit */
    printEnd(); 
    input.close();
    return EXIT_SUCCESS;
    
}