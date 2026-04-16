#ifndef Cache_Set_H
#define Cache_Set_H
#include "cache_block.h"
#include <vector>

struct cache_set{
  std::vector<cache_block> ways;

  cache_set (int associativity) : ways(associativity){}

  void updateLRU(int wayIndex){
    updateSetLRU();
    ways[wayIndex].lru_counter = 0;
    ways[wayIndex].dirty = false;
  };

  void updateSetLRU(){
    for(int i = 0; i < ways.size(); i++){
      ways[i].lru_counter++;
    }
  }
};

#endif