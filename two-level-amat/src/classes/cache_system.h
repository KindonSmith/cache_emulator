#ifndef Cache_System_H
#define Cache_System_H
#include "cache.h"

using namespace std;

class Cache_System
{
private:
  /* data */
  map<int, vector<Cache>> cache_list;
  int num_caches = 2;
  Stats system_stats;
public:
  Cache_System();
  vector<Cache> get_cache_list();
  void insert_cache(cache_config config);
  lookup_result look_up(uint32_t address, string directive);
};


Cache_System::Cache_System(){}

/*vector<Cache> Cache_System::get_cache_list(){
  return cache_list;
}*/

void Cache_System::insert_cache(cache_config config){
  Cache cache(config);
  // if no key for 1 is found, add key:cache for 1
  // if no key for 2, add key:cache for 2
  if (cache_list.find(1) == cache_list.end()){
      cache_list[1].push_back(cache);
  } else if (cache_list.find(2) == cache_list.end()){
      cache_list[2].push_back(cache);
  } else {
    // more than 2 caches being pushed. unsupported.
    cout << "Warning: Only 2 caches supported, but more than 2 configurations found." << endl;
  }
}




#endif