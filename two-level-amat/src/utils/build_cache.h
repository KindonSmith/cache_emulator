#ifndef Build_Cache_H
#define Build_Cache_H
#include "../classes/cache.h"


void buildCacheConfig(map<string, string> configMap, Cache_System &system){
  cache_config built_config(configMap);
  system.insert_cache(built_config);


};

#endif