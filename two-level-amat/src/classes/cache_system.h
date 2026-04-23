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
  vector<Cache> get_caches();
};

Cache_System::Cache_System(vector<Cache> cacheList)
{
  cache_list = cacheList;
}

vector<Cache> Cache_System::get_caches(){
  return cache_list;
}




#endif