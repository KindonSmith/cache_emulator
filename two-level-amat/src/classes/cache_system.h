#ifndef Cache_System_H
#define Cache_System_H
#include "cache.h"

using namespace std;

class Cache_System
{
private:
  /* data */
  int num_caches = 2;
  int memory_access_time{};
  int total_accesses{};

  map<int, Cache> cache_list;
  map<int, Stats> cache_stats;
  vector<Instruction> instruction_list;
public:
  Cache_System();
  map<int, Cache>& get_cache_list();
  void insert_cache(cache_config config);
  lookup_result look_up(Instruction instruction, Cache& cache);
  void set_instructions(vector<Instruction> _instructions);
  vector<Instruction> get_instruction_list();
  void simulate();
  void set_access_time(int _access_time);
  void insert_instruction(Instruction instruction);
  void print_system_config();
  void print_simulated_stats();
};


#endif