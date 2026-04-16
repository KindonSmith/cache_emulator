#ifndef Cache_H
#define Cache_H
#include "cache_set.h"
#include "cache_config.h"
#include "stats.h"
#include <vector>
#include <cmath>
#include <string>
#include <iostream>

using namespace std;

struct Cache{
  std::vector<cache_set> sets{};
  int cache_size{};
  int block_size{};
  int associativity{};
  WriteHitPolicy write_hit;
  WriteMissPolicy write_miss;
  Stats stats{};
  
  uint32_t offsetBits{};
  uint32_t indexBits{};
  uint32_t tagBits{};


  /**
   * @brief Construct a new Cache object
   * 
   * @param set default template for all sets on cache init
   * @param cacheSize size of the cache in kb
   * @param blockSize size of each block in kb
   * @param assoc slicing of the sets ()
   */
  Cache(cache_set set, int cacheSize, int blockSize, int assoc)
        : cache_size(cacheSize), block_size(blockSize), associativity(assoc)
  {
    int numSets = cacheSize / blockSize / assoc;
    for(int i = 0; i < numSets; i++){
      sets.push_back(set);
    }
    while (blockSize > 1){
      blockSize >>= 1;
      offsetBits++;
    }
    while (numSets > 1){
      numSets >>= 1;
      indexBits++;
    }
    tagBits = 32u - indexBits - offsetBits;
  }
  Cache(cache_config config)
        : cache_size(config.cache_size), block_size(config.block_size), associativity(config.associativity),
        write_hit(config.write_hit), write_miss(config.write_miss)
  {
    cache_set set(config.associativity);
    int numSets = config.cache_size / config.block_size / config.associativity;
    for(int i = 0; i < numSets; i++){
      sets.push_back(set);
    }
    while (config.block_size > 1){
      config.block_size >>= 1;
      offsetBits++;
    }
    while (numSets > 1){
      numSets >>= 1;
      indexBits++;
    }
    tagBits = 32u - indexBits - offsetBits;
  }
  
  /**
   * @brief Get the Index object from an address. Uses cache defined
   * indexBits for mask and offset for bitshift to correct index
   * @param address instruction address, fixed sized unsigned int
   * @return index of the instruction's set in cache (N-bit masked index)
   */
  uint32_t getIndex(uint32_t address){
    uint32_t mask{};
    for(int i = 0; i < indexBits; i++){
      mask <<= 1;
      mask += 1;
    }
    return address >> offsetBits & mask;
  }
  
  /**
   * @brief Get the Tag object
   * 
   * @param address instruction address, fixed sized unsigned int
   * @return uint32_t tag bits of instruction, used to verify against way
   */
  uint32_t getTag(uint32_t address){
    return address >> indexBits + offsetBits;
  }

  /**
   * @brief Given an address, return 1 on hit, 0 on miss, and update LRU
   * for a set's instructions.
   * 
   * @param address 
   * @return int 
   */
  int lookUp(uint32_t address){
    int lruMaxIndex{};
    uint32_t index = getIndex(address);
    uint32_t tag = getTag(address);
    for(int wayIndex = 0; wayIndex < associativity; wayIndex++){
      if(sets[index].ways[wayIndex].valid){
        if (sets[index].ways[wayIndex].tag == tag){
          sets[index].updateLRU(wayIndex);
          return 1;
        }
      }

      int curLRU {sets[index].ways[wayIndex].lru_counter};
      int maxLRU {sets[index].ways[lruMaxIndex].lru_counter};
      if(curLRU > maxLRU){
        lruMaxIndex = wayIndex;
      }
    }

    // if we reach here, we've checked all necessary ways and can write
    // we've also missed, so we return 0
    sets[index].ways[lruMaxIndex].valid = true;
    sets[index].ways[lruMaxIndex].tag = tag;
    sets[index].updateLRU(lruMaxIndex);

    return 0;
  }
  /**
   * @brief 
   * Cache size:      1024 bytes
   * Block size:      32 bytes
   * Associativity:   4 ways
   * Number of sets:  8
   * Write hit:       write-back
   * Write miss:      write-allocate
   * 
   */
  void print() {
    string w = "  ";
    cout << "Cache Configuration:" << endl;
    cout << w << "Cache size: " << w << cache_size << "bytes" << endl;
    cout << w << "Block sze: " << w << block_size << "bytes" << endl;
    cout << w << "Associativity: " << w << associativity << "ways" << endl;
    cout << w << "Number of sets: " << w << sets.size() << endl;
    cout << w << "Write hit: " << w << (write_hit == WriteHitPolicy::WRITE_BACK ? "write-back" : "write-throu") << endl;
    cout << w << "Write miss: " << w << (write_miss == WriteMissPolicy::WRITE_ALLOCATE ? "write-allocate" : "no-write-allocate") << endl;
  }
};

#endif