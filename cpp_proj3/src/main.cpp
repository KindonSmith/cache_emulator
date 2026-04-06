#include <iostream>
#include <fstream>
#include <string>

#include "structs/cache_block.h"
#include "structs/cache_set.h"

using namespace std;

vector<uint32_t> parseAddresses(char* inputFileName){
  string inputFileRoute = "input/";
  string inputFileString = inputFileRoute + string(inputFileName);
  ifstream inputFile(inputFileString);
  uint32_t address{};
  vector<uint32_t> addressList{};

  if (inputFile.is_open()){
    while(inputFile >> std::hex >> address){
      addressList.push_back(address);
    }
    inputFile.close();
  }else{
    cout << "Unable to open input file." << endl;
  }
  return addressList;
}

vector<cache_set> buildCache(char* cacheSizeInput, char* blockSizeInput, char* associativityInput){
  // cache size - total memory
  // block size - cache size / block size gives us number of sets
  // associativity - set size
  int cacheSize = atoi(cacheSizeInput);
  int blockSize = atoi(blockSizeInput);
  int associativity = atoi(associativityInput);

  // if block size is 0, we can't divide by 0, throw exception
  if (blockSize == 0) {
    cout << "Invalid block size of 0." << endl;
    throw runtime_error("Division by zero.");
  }

  int numSets = cacheSize / blockSize / associativity;
  vector<cache_set> cache(numSets, cache_set(associativity));
  // cache with correct number of sets, each of associative size  g++ -g main.cpp cache.cpp stats.cpp -o cache_sim
  return cache;
}

int main(int argc, char* argv[]) {
  // args: cache size (bytes), block size (bytes), 
  // associativity, trace file path
  if (argc != 5){
    cout << "Invalid number of arguments." << endl;
    return 0;
  }
  vector<cache_set> cache = buildCache(argv[1], argv[2], argv[3]);
  vector<uint32_t> addressList = parseAddresses(argv[4]);

}