#ifndef Cache_H
#define Cache_H
#include "cache_set.h"
#include <vector>
#include <cmath>

struct Cache{
  std::vector<cache_set> sets{};
  int cache_size{};
  int block_size{};
  int associativity{};
  
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
};

#endif