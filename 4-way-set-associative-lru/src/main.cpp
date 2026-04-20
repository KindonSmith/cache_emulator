#include <iostream>
#include <fstream>
#include <string>

#include "structs/cache_block.h"
#include "structs/cache_set.h"

using namespace std;

// extracts index with 5 bit shift and 3 bit mask.
// function targets bits 5-7 per spec, pulling memory address
uint32_t extractIndex (uint32_t inputAddress){
  return inputAddress >> 5 & 0x7;
}

// extracts tag with 8 bit shift (bits 8-31)
uint32_t extractTag (uint32_t inputAddress){
  return inputAddress >> 8;
}


// this cachelookup varies from proj 1 because it uses a 2d array
// a cache containing sets of ways
// simulator doesn't technically check in parallel, but that's
// a hardware detail, not important to this implementation
int cacheLookup(cache_set cache[], uint32_t index, uint32_t tag){
  int lruMaxIndex = 0;
  for(int wayIndex = 0; wayIndex < 4; wayIndex++){
    cache_block way = cache[index].ways[wayIndex];
    if (way.valid){
      if (way.tag == tag){
        cache[index].updateLRU(wayIndex);
        return 1;
      }
    }
    // we only write on miss
    // we only traverse all the way through on miss
    // therefore we can use reads to find the max lru.
    if (way.lru_counter > cache[index].ways[lruMaxIndex].lru_counter) {
      lruMaxIndex = wayIndex;
    }
  }
  // if we reach this point, we've missed and can return 0 after 
  // writing to cache at lruMaxIndex
  cache[index].ways[lruMaxIndex].tag = tag;
  cache[index].ways[lruMaxIndex].valid = true;
  cache[index].updateLRU(lruMaxIndex);

  return 0;
}

void readInputFile(cache_set cache[], int& hits, int& queries){
  ifstream inputFile ("input/input.txt");
  uint32_t address;
  uint32_t index;
  uint32_t tag;
  if (inputFile.is_open()){
    while (inputFile >> std::hex >> address){
      index = extractIndex(address);
      tag = extractTag(address);
      //cout << "Index: " << index << "| Tag: " << tag << endl;
      hits += cacheLookup(cache, index, tag);
      queries++;
    }
    inputFile.close();
  }else{
    cout << "Unable to open file." << endl;
    throw runtime_error("Unable to open input file.");
  }
}



int main() {
  //cache_block cache[32]{};
  // cache contains 8 sets of 4 ways each. index is 0-7, representable with 3 index bits
  cache_set cache[8]{};
  int hits{};
  int queries{};
  readInputFile(cache, hits, queries);
  cout << "Total accesses: " << queries << endl;
  cout << "Hits: " << hits << endl;
  cout << "Misses: " << queries - hits << endl;
  cout << "Hit Rate: " << (double)hits / queries * 100<< "%" << endl;
  


  return 0;
}

