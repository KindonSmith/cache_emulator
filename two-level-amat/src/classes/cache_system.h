#ifndef Cache_System_H
#define Cache_System_H
#include "cache.h"

using namespace std;

class Cache_System
{
private:
  /* data */
  vector<Cache> cache_list;
public:
  Cache_System(vector<Cache> cacheList);
  Cache_System();
  vector<Cache> get_caches();
  void insert_cache(cache_config config);
};

Cache_System::Cache_System(vector<Cache> cacheList)
{
  cache_list = cacheList;
}
Cache_System::Cache_System(){}

vector<Cache> Cache_System::get_caches(){
  return cache_list;
}

void Cache_System::insert_cache(cache_config config){

}




#endif