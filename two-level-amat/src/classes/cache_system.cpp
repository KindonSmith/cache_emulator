#include "cache_system.h"

Cache_System::Cache_System(){}

void Cache_System::insert_cache(cache_config config){
  Cache cache(config);
  if (cache_list.find(1) == cache_list.end()){
      cache_list.emplace(1, cache);
      cache_stats.emplace(1, Stats());
  } else if (cache_list.find(2) == cache_list.end()){
      cache_list.emplace(2, cache);
      cache_stats.emplace(2, Stats());
  } else {
    cout << "Warning: Only 2 caches supported, but more than 2 configurations found." << endl;
  }
}

void Cache_System::set_instructions(vector<Instruction> _instructions){
  instruction_list = _instructions;
}

map<int, Cache>& Cache_System::get_cache_list(){
  return cache_list;
}

vector<Instruction> Cache_System::get_instruction_list(){
  return instruction_list;
}

lookup_result Cache_System::look_up(Instruction instruction, Cache& cache){
  return cache.lookUp(instruction);
}

void Cache_System::set_access_time(int _access_time){
  memory_access_time = _access_time;
}

void Cache_System::validate_result(lookup_result result, int cache_index, string directive){
  auto& caches = get_cache_list();
  if (caches.find(cache_index) == caches.end()){
    cout << "validate_result. Cannot check result against cache that does not exist: " << cache_index << "." << endl;
  }
  // on hit
  if (result.hit){
    cache_stats[cache_index].hits++;
    if (directive == "W"){
      cache_stats[cache_index].writes++;
      if (caches.at(cache_index).write_hit == WriteHitPolicy::WRITE_THROUGH){
        cache_stats[cache_index].writes_to_next_level++;
      }
    }else if (directive == "R"){
      cache_stats[cache_index].reads++;
    }
  // on miss 
  }else if (!result.hit){
    cache_stats[cache_index].misses++;
    if (directive == "W"){

    }else if (directive == "R"){

    }
  }

}

void Cache_System::insert_instruction(Instruction _instruction){
  if (_instruction.directive == "R" || _instruction.directive == "W"){
    instruction_list.push_back(_instruction);
  }else{
    cout << "Skipping invalid instruction directive: " << _instruction.directive << endl;
  }
}


void Cache_System::simulate(){
  auto instructions = get_instruction_list();
  auto& caches = get_cache_list();
  cout << "Simulating..." << endl;
  
  for(int i = 0; i < instructions.size(); i++){
    int cache_index = 1;
    if(caches.find(cache_index) != caches.end()){
      lookup_result result = look_up(instructions[i], caches.at(cache_index));
      validate_result(result, cache_index, instructions[i].directive);
      if (!result.hit) {
        cache_index++;
        result = look_up(instructions[i], caches.at(cache_index));
        validate_result(result, cache_index, instructions[i].directive);
      }

    }else{
      throw runtime_error("Could not find valid cache. Please check cache index configuration.");
    }
  }
  cout << cache_stats[1].hits << endl;
  cout << cache_stats[1].misses << endl;
  cout << cache_stats[1].writebacks << endl;

}