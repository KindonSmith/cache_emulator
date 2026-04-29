#include "cache.h"
#include "cache_system.h"

lookup_result Cache_System::look_up(uint32_t address, string directive, Cache cache){
  return cache.lookUp(address, directive);
}


void Cache_System::simulate(){}