#include "cache_system.h"

Cache_System::Cache_System(){}

void Cache_System::insert_cache(cache_config config){
  Cache cache(config);
  if (cache_list.find(1) == cache_list.end()){
      cache_list[1] = cache;
  } else if (cache_list.find(2) == cache_list.end()){
      cache_list[2]= cache;
  } else {
    cout << "Warning: Only 2 caches supported, but more than 2 configurations found." << endl;
  }
}

void Cache_System::set_instructions(vector<Instruction> _instructions){
  instruction_list = _instructions;
}

vector<Instruction> Cache_System::get_instruction_list(){
  return instruction_list;
}

lookup_result Cache_System::look_up(uint32_t address, string directive, Cache cache){
  return cache.lookUp(address, directive);
}


void Cache_System::simulate(){
  auto instructions = get_instruction_list();
  auto caches = get_cache_list();
  
  for(int i = 0; i < instructions.size(); i++){
    if(caches.find(1) != caches.end()){

    }
  }

}