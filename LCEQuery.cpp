#include <fstream>
#include <cstdlib>
#include <iostream>
#include "util.hpp"
using namespace std;




ifstream input;


/* size of input file in byte*/
uint64_t fileSize; 





uint64_t phiToI(uint64_t i) {

    
    return 0;
}


uint64_t phiIToJ(uint64_t i, uint64_t j) {
    
    return 0;
}





long naiveLCE(uint64_t index1, uint64_t index2) {
    return 0;
}


long prezzaLCE(uint64_t index1,uint64_t index2) {
    return 0;
}



uint64_t randomIndex() {
    uint64_t randIndex = rand();
    randIndex <<= 32;
    randIndex += rand();
    
    return randIndex;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: LCEQuery FILE_PATH NUMBER_OF_RANDOM_QUERIES\n"
        << "Example: LCEQuery RKFenglish.s 5000\n";
        return EXIT_FAILURE;
    }
    uint64_t numberOfQueries = atoi(argv[3]);
    
    
    fileSize = util::calculateSizeOfInputFile(&input);
        
        
    
    srand(time(0));
    uint64_t randIndex1, randIndex2;
    for (unsigned int i = 0; i < numberOfQueries; i++) {
        randIndex1 = randomIndex();
        randIndex2 = randomIndex();
        naiveLCE(randIndex1, randIndex2);
    }
    
    
    //CONSTRUCT LCE DS AND
    for (unsigned int i = 0; i < numberOfQueries; i++) {
        randIndex1 = randomIndex();
        randIndex2 = randomIndex();
        prezzaLCE(randIndex1, randIndex2);
    }
    
    
    
    
}