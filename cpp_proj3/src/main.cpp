#include <iostream>
#include <fstream>
#include <string>

#include "structs/cache_block.h"
#include "structs/cache_set.h"
#include "structs/cache.h"

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

Cache buildCacheUtility(char* cacheSizeInput, char* blockSizeInput, char* associativityInput){
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

  // Cache cachePrimitive(numSets, cache_set(associativity));
  // cache with correct number of sets, each of associative size  g++ -g main.cpp cache.cpp stats.cpp -o cache_sim
  return Cache(cache_set(associativity), cacheSize, blockSize, associativity);
}

void checkCache(Cache &cache, const vector<uint32_t> &addressList, int &hits)
{
  for(int i=0; i<addressList.size(); i++){
    hits += cache.lookUp(addressList[i]);
  }
}


int main(int argc, char* argv[]) {
  // args: cache size (bytes), block size (bytes), 
  // associativity, trace file path
  if (argc != 5){
    cout << "Invalid number of arguments." << endl;
    return 0;
  }
  int hits{};
  Cache cache = buildCacheUtility(argv[1], argv[2], argv[3]);
  vector<uint32_t> addressList = parseAddresses(argv[4]);
  checkCache(cache, addressList, hits);
  int queries = addressList.size();
  cout << "Total accesses: " << queries << endl;
  cout << "Hits: " << hits << endl;
  cout << "Misses: " << queries - hits << endl;
  cout << "Hit Rate: " << (double)hits / queries * 100<< "%" << endl;
}