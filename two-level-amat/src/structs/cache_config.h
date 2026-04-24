
#ifndef Cache_Config_H
#define Cache_Config_H
#include <string>
#include <map>
#include <iostream>

using namespace std;

enum class WriteHitPolicy {WRITE_BACK, WRITE_THROUGH};
enum class WriteMissPolicy {WRITE_ALLOCATE, NO_WRITE_ALLOCATE};

struct cache_config{
  int cache_size{};
  int block_size{};
  int associativity{};
  WriteHitPolicy write_hit{};
  WriteMissPolicy write_miss{};

  cache_config(){};
  cache_config(int cacheSize, int blockSize, int assoc, WriteHitPolicy writeHit, WriteMissPolicy writeMiss) : cache_size(cacheSize),
              block_size(blockSize), associativity(assoc), write_hit(writeHit), write_miss(writeMiss){};
  cache_config(map<string, string> config_map){
    for (auto const& [subkey, subval] : config_map){
      cout << subkey << subval << endl;
    }  
    cout << "CACHE_DONE" << endl;
  }
};

#endif