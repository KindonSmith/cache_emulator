#ifndef Cache_Set_H
#define Cache_Set_H
#include "cache_block.h"

struct cache_set{
  cache_block ways[4]{};
  void updateLRU(int wayIndex){
    updateSetLRU();
    ways[wayIndex].lru_counter = 0;
  };
  void updateSetLRU(){
    for(int i = 0; i < 4; i++){
      ways[i].lru_counter++;
    }
  }
};

#endif