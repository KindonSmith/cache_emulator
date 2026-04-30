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
  vector<Instruction> instruction_list;
public:
  Cache_System();
  map<int, vector<Cache>> get_cache_list();
  void insert_cache(cache_config config);
  lookup_result look_up(uint32_t address, string directive, Cache cache);
  void set_instructions(vector<Instruction> _instructions);
  vector<Instruction> get_instruction_list();
  void simulate();
};


#endif