#include <iostream>
#include <fstream>
#include <string>

#include "structs/cache_block.h"

using namespace std;

// extracts index with 5 bit shift and 5 bit mask.
// function targets bits 5-9 per spec, pulling memory address
uint32_t extractIndex (uint32_t inputAddress){
  return inputAddress >> 5 & 0x1F;
}

// extracts tag with 10 bit shift (bits 10-31)
uint32_t extractTag (uint32_t inputAddress){
  return inputAddress >> 10;
}

// cacheLookup takes in a cache, index, and tag data
// quick lookup uses index to verify validity then checks data against ag
// if not valid, we miss and cache
// if valid, we hit. for this assignment, just return 1.
int cacheLookup(cache_block cache[], uint32_t index, uint32_t tag){
  if (cache[index].valid){
    if (cache[index].tag == tag){
      return 1;
    }
  }
  // if invalid or tag doesn't match, we have missed
  // we should immediately cache and return 0
  cache[index].tag = tag;
  cache[index].valid = true;
 
  return 0;

}


// takes a cache_block array (arrays default to reference) and reference
// to hits and queries counters. Since cachelookup returns 1 on hit,
// increment hits by cachelookup return value and queries 1 per read.
void readInputFile (cache_block cache[], int& hits, int& queries){
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
  }
}



int main() {
  cache_block cache[32]{};
  int hits{};
  int queries{};
  readInputFile(cache, hits, queries);
  cout << "Total accesses: " << queries << endl;
  cout << "Hits: " << hits << endl;
  cout << "Misses: " << queries - hits << endl;
  cout << "Hit Rate: " << (double)hits / queries * 100<< "%" << endl;
  


  return 0;
}

