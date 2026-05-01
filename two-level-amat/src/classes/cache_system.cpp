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
    // increment on hit/miss and read/write
    l1.hit ? cache_stats[1].hits++ : cache_stats[1].misses++;
    dir == "R" ? cache_stats[1].reads++ : cache_stats[1].writes++;

    // L1 eviction
    if (l1.writeback_occurred) {
      cache_stats[1].writebacks++;
      caches.at(2).dirty_block(l1.wb_victim_address);
    }

    // L1 write hit -> propagate to L2
    if (l1.hit && dir == "W" && caches.at(1).write_hit == WriteHitPolicy::WRITE_THROUGH) {
      caches.at(2).propagate_write(addr);
      cache_stats[1].writes_to_next_level++;
    }

    // L1 miss
    if (!l1.hit) {
      // L2 lookup
      lookup_result l2 = caches.at(2).lookUp(instructions[i]);
      cache_stats[2].total_accesses++;
      // increment hit/miss and read/write
      l2.hit ? cache_stats[2].hits++ : cache_stats[2].misses++;
      dir == "R" ? cache_stats[2].reads++ : cache_stats[2].writes++;

      // L2 eviction
      if (l2.writeback_occurred) {
        cache_stats[2].writebacks++;
      }

      if (l2.hit && dir == "W" && caches.at(2).write_hit == WriteHitPolicy::WRITE_THROUGH){
        // conceptually, this would be a write to ram.
        // caches.at(3).propagate_write(addr)
        cache_stats[2].writes_to_next_level++;
        
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

void Cache_System::print_analysis(){
  // AMAT = Hit Time + Miss Rate * Miss Penalty
  // 2 level AMAT = L1 Hit Time + L1 Miss Rate * (L2 Hit Time + L2 Miss Rate * memory_access_time)
  auto l1_stats = cache_stats.at(1);
  float l1_hit_time = cache_list.at(1).hit_time;
  float l1_miss_rate = l1_stats.total_accesses > 0 ? ((float)l1_stats.misses / (float)l1_stats.total_accesses) : 0.0f;
  float l1_hit_rate = 1.0f - l1_miss_rate;

  auto l2_stats = cache_stats.at(2);
  float l2_hit_time = cache_list.at(2).hit_time;
  float l2_miss_rate = l2_stats.total_accesses > 0 ? ((float)l2_stats.misses / (float)l2_stats.total_accesses) : 0.0f;

  float AMAT = l1_hit_time + l1_miss_rate * (l2_hit_time + l2_miss_rate * (float)memory_access_time);

  cout << fixed << setprecision(2);
  cout << "\n=== Results ===" << endl;

  cout << "L1:" << endl;
  cout << "  Accesses: " << l1_stats.total_accesses << endl;
  cout << "  Hits: " << l1_stats.hits << "     Misses: " << l1_stats.misses << endl;
  cout << "  Hit rate: " << (l1_hit_rate * 100.0f) << "%" << endl;
  cout << "  Writebacks to L2: " << l1_stats.writebacks << endl;

  cout << "\nL2:" << endl;
  cout << "  Accesses: " << l2_stats.total_accesses << endl;
  cout << "  Hits: " << l2_stats.hits << "     Misses: " << l2_stats.misses << endl;
  cout << "  Hit rate: " << ((1.0f - l2_miss_rate) * 100.0f) << "%" << endl;
  cout << "  Writebacks to memory: " << l2_stats.writebacks << endl;

  cout << "\nAMAT: " << l1_hit_time << " + " << l1_miss_rate << " * (" << l2_hit_time << " + " << l2_miss_rate << " * " << memory_access_time << ") = " << AMAT << " cycles" << endl;
}

