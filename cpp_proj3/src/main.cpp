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

Cache buildCache(char* cacheSizeInput, char* blockSizeInput, char* associativityInput){
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

uint32_t getIndex(uint32_t address, uint32_t bits, uint32_t offset){
  uint32_t mask{};
  for(int i = 0; i < bits; i++){
    mask <<= 1;
    mask += 1;
  }
  return address >> offset & mask;
}
uint32_t getTag(uint32_t address, uint32_t bits){
  return address >> bits;
}

/*int cacheLookup(Cache &cache, uint32_t address){
  /*int lruMaxIndex{};
  uint32_t index = cache.getIndex(address);
  uint32_t tag = cache.getTag(address);
  for(int wayIndex = 0; wayIndex < cache.associativity; wayIndex++){
    if(cache.sets[index].ways[wayIndex].valid){
      if (cache.sets[index].ways[wayIndex].tag == tag){
        cache.sets[index].updateLRU(wayIndex);
        return 1;
      }
    }

    int curLRU {cache.sets[index].ways[wayIndex].lru_counter};
    int maxLRU {cache.sets[index].ways[lruMaxIndex].lru_counter};
    if(curLRU > maxLRU){
      lruMaxIndex = wayIndex;
    }
  }

  // if we reach here, we've checked all necessary ways and can write
  // we've also missed, so we return 0
  cache.sets[index].ways[lruMaxIndex].valid = true;
  cache.sets[index].ways[lruMaxIndex].tag = tag;
  cache.sets[index].updateLRU(lruMaxIndex);

  return 0;
}*/

void checkCache(Cache &cache, vector<uint32_t> addressList, int &hits)
{
  for(int i=0; i<addressList.size(); i++){
    //uint32_t index = getIndex(addressList[i], cache.indexBits, cache.offsetBits);
    //uint32_t tag = getTag(addressList[i], cache.offsetBits + cache.indexBits);
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
  Cache cache = buildCache(argv[1], argv[2], argv[3]);
  vector<uint32_t> addressList = parseAddresses(argv[4]);
  checkCache(cache, addressList, hits);
  int queries = addressList.size();
  cout << "Total accesses: " << queries << endl;
  cout << "Hits: " << hits << endl;
  cout << "Misses: " << queries - hits << endl;
  cout << "Hit Rate: " << (double)hits / queries * 100<< "%" << endl;
}