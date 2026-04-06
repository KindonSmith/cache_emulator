#include <iostream>
#include <fstream>
#include <string>

#include "structs/cache_block.h"
#include "structs/cache_set.h"

using namespace std;

void parseInput(char* inputFileName){
  string inputFileRoute = "input/";
  string inputFileString = inputFileRoute + string(inputFileName);
  //cout <<"InputFileString: " << inputFileString << endl;
  ifstream inputFile(inputFileString);
  uint32_t address{};
  if (inputFile.is_open()){
    while(inputFile >> std::hex >> address){
      //cout << "Input file is being read, address: " << address << endl;
    }

  }
}

vector<cache_set> buildCache(char* cacheSizeInput, char* blockSizeInput, char* associativityInput){
  // parse cache and block size from char* to int
  int cacheSize = atoi(cacheSizeInput);
  int blockSize = atoi(blockSizeInput);
  // if block size is 0, we can't divide by 0, throw exception
  if (blockSize == 0) {
    cout << "Invalid block size of 0." << endl;
    throw runtime_error("Division by zero.");
  }
  int numSets = cacheSize / blockSize;

  int associativity = atoi(associativityInput);

}

int main(int argc, char* argv[]) {
  // args: cache size (bytes), block size (bytes), 
  // associativity, trace file path
  if (argc != 5){
    cout << "Invalid number of arguments." << endl;
    return 0;
  }
  vector<cache_set> cache = buildCache(argv[1], argv[2], argv[3]);
}