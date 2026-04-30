#ifndef Cache_H
#define Cache_H
#include "../structs/cache_structs.h"
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

class Cache{
  private:
  public:
    vector<cache_set> sets{};
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

  bool matchingTags(uint32_t tag1, uint32_t tag2){
    return tag1 == tag2;
  }

  void updateBlock(cache_block &block){
    if(block.dirty) {
      // stats.writes_to_next_level++;
      // stats.writebacks++;
      block.dirty = false;
    }
  }

  void loadBlock(cache_block &block, uint32_t tag){
    block.valid = true;
    block.tag = tag;
    block.dirty = false;
  }

  void wa_miss(cache_block &block, uint32_t tag){
    loadBlock(block, tag);
    if(write_hit == WriteHitPolicy::WRITE_BACK) block.dirty = true;
  }
  void nwa_miss(cache_block &block){
    // stats.writes_to_next_level++;
  }

  void instruction_miss(cache_block &block, uint32_t tag, string directive){
    // stats.misses++;
    // updateBlock(block);
    
    if(directive == "W"){
      // stats.writes++;
      if(write_miss == WriteMissPolicy::WRITE_ALLOCATE) wa_miss(block, tag);
      if(write_miss == WriteMissPolicy::NO_WRITE_ALLOCATE) nwa_miss(block);
    }else if(directive == "R"){
      // stats.reads++;
      loadBlock(block, tag);
    }
  }

  void wb_hit(cache_block &block){
    if (!block.dirty) block.dirty = true;
  }

  void wt_hit(){
    // stats.writes_to_next_level++;
  }

  void instruction_hit(cache_block &block, uint32_t tag, string directive){
    //stats.hits++;
    if(directive == "W"){
      //stats.writes++;
      if(write_hit == WriteHitPolicy::WRITE_BACK) wb_hit(block);
      if(write_hit == WriteHitPolicy::WRITE_THROUGH) wt_hit();
    }else if(directive == "R"){
      //stats.reads++;
    }

  }

  /**
   * @brief Given an address, return 1 on hit, 0 on miss, and update LRU
   * for a set's instructions.
   * 
   * @param address 
   * @return int 
   */
  lookup_result lookUp(Instruction instruction){
    int lruMaxIndex{};
    uint32_t index = getIndex(instruction.address);
    uint32_t tag = getTag(instruction.address);

    // stats.total_accesses++;

    for(int wayIndex = 0; wayIndex < associativity; wayIndex++){
      if(sets[index].ways[wayIndex].valid){
        if (matchingTags(sets[index].ways[wayIndex].tag, tag)){
          // updateBlock(index, wayIndex);
          instruction_hit(sets[index].ways[wayIndex], tag, instruction.directive);
          sets[index].updateLRU(wayIndex);
          return lookup_result(true, false, &sets[index].ways[wayIndex]);
        }
      }

      int curLRU {sets[index].ways[wayIndex].lru_counter};
      int maxLRU {sets[index].ways[lruMaxIndex].lru_counter};
      if(curLRU > maxLRU){
        lruMaxIndex = wayIndex;
      }
    }
     /*
    // wb + nwa is an incoherent combination (write to cache later + only write to cache now)
    // therefore a slot can never be dirty on NWA miss, so this should always result in a
    // false on NWA miss. 
    */
    bool block_was_dirty = sets[index].ways[lruMaxIndex].dirty;
    instruction_miss(sets[index].ways[lruMaxIndex], tag, instruction.directive);
    if(write_miss != WriteMissPolicy::NO_WRITE_ALLOCATE) {
      sets[index].updateLRU(lruMaxIndex);
    }

    return lookup_result(false, block_was_dirty, &sets[index].ways[lruMaxIndex]);
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
  void printConfig() {
    string w = "  ";
    cout << "Cache Configuration:            " << endl;
    cout << w << "Cache size:                " << w << cache_size << " bytes" << endl;
    cout << w << "Block sze:                 " << w << block_size << " bytes" << endl;
    cout << w << "Associativity:             " << w << associativity << " ways" << endl;
    cout << w << "Number of sets:            " << w << sets.size() << endl;
    cout << w << "Write hit:                 " << w << (write_hit == WriteHitPolicy::WRITE_BACK ? "write-back" : "write-throu") << endl;
    cout << w << "Write miss:                " << w << (write_miss == WriteMissPolicy::WRITE_ALLOCATE ? "write-allocate" : "no-write-allocate") << endl;
  }

  void printStats() {
    string w = "  ";
    float hitRate = stats.total_accesses > 0 ? (100.0f * stats.hits / stats.total_accesses) : 0.0f;
    cout << "Results:" << endl;
    cout << w << "Total accesses:       " << stats.total_accesses << endl;
    cout << w << "Reads:                " << stats.reads << endl;
    cout << w << "Writes:               " << stats.writes << endl;
    cout << w << "Hits:                 " << stats.hits << endl;
    cout << w << "Misses:               " << stats.misses << endl;
    cout << w << "Hit rate:             " << fixed << setprecision(2) << hitRate << "%" << endl;
    cout << w << "Writebacks:           " << stats.writebacks << endl;
    cout << w << "Writes to next level: " << stats.writes_to_next_level << endl;
  }
};

#endif