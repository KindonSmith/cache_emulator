#include "cache_system.h"

Cache_System::Cache_System(){}

void Cache_System::insert_cache(cache_config config){
  Cache cache(config);
  if (cache_list.find(1) == cache_list.end()){
      cache_list[1].push_back(cache);
  } else if (cache_list.find(2) == cache_list.end()){
      cache_list[2].push_back(cache);
  } else {
    cout << "Warning: Only 2 caches supported, but more than 2 configurations found." << endl;
  }
}

void Cache_System::set_instructions(vector<Instruction> _instructions){
  instruction_list = _instructions;
}

vector<Instruction> Cache_System::get_instructions(){
  return instruction_list;
}

lookup_result Cache_System::look_up(uint32_t address, string directive, Cache cache){
  return cache.lookUp(address, directive);
}


void Cache_System::simulate(){
  auto instructions = get_instructions();
  for(int i = 0; i < instructions.size(); i++){
    // for each instruction, we need to lookup in l1. on miss, we lookup l2.
    // 
  }

}