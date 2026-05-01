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
    total_accesses++;
    string dir = instructions[i].directive;
    uint32_t addr = instructions[i].address;

    // L1 lookup
    lookup_result l1 = look_up(instructions[i], caches.at(1));
    cache_stats[1].total_accesses++;
    l1.hit ? cache_stats[1].hits++ : cache_stats[1].misses++;
    dir == "R" ? cache_stats[1].reads++ : cache_stats[1].writes++;

    // L1 dirty eviction -> mark block dirty in L2
    if (l1.writeback_occurred) {
      cache_stats[1].writebacks++;
      caches.at(2).dirty_block(l1.wb_victim_address);
    }

    // L1 WT write hit -> propagate to L2
    if (l1.hit && dir == "W" && caches.at(1).write_hit == WriteHitPolicy::WRITE_THROUGH) {
      caches.at(2).propagate_write(addr);
      cache_stats[1].writes_to_next_level++;
    }

    if (!l1.hit) {
      // L2 lookup
      lookup_result l2 = caches.at(2).lookUp(instructions[i]);
      cache_stats[2].total_accesses++;
      l2.hit ? cache_stats[2].hits++ : cache_stats[2].misses++;
      dir == "R" ? cache_stats[2].reads++ : cache_stats[2].writes++;

      // L2 dirty eviction -> memory writeback
      if (l2.writeback_occurred) {
        cache_stats[2].writebacks++;
      }
    }
  }
}

void Cache_System::print_system_config(){
  auto cache_list = get_cache_list();
  cache_list.at(1).printConfig();
  cache_list.at(2).printConfig();
}

void Cache_System::print_simulated_stats(){
  cout << "  " << "Total accesses:       " << total_accesses << endl;
  cache_stats.at(1).print();
  cache_stats.at(2).print();
}

