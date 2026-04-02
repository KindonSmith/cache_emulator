#include <cstdint>

#ifndef Cache_Block_H
#define Cache_Block_H

struct cache_block{
  uint32_t tag;
  bool valid;
};

#endif