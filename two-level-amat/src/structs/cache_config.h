
#ifndef Cache_Config_H
#define Cache_Config_H
#include <string>
#include <map>
#include <iostream>
#include <functional>

using namespace std;

enum class WriteHitPolicy {WRITE_BACK, WRITE_THROUGH};
enum class WriteMissPolicy {WRITE_ALLOCATE, NO_WRITE_ALLOCATE};

struct cache_config{
  int cache_size{};
  int block_size{};
  int associativity{};
  int hit_time{};
  WriteHitPolicy write_hit{};
  WriteMissPolicy write_miss{};

  WriteHitPolicy parseWriteHit(string s) { 
    if (s == "wb") return WriteHitPolicy::WRITE_BACK;
    if (s == "wt") return WriteHitPolicy::WRITE_THROUGH;
    throw runtime_error("Invalid Write Hit Policy" + s + ".\n");
  }

  WriteMissPolicy parseWriteMiss(string s) {
    if (s == "wa") return WriteMissPolicy::WRITE_ALLOCATE;
    if (s == "nwa") return WriteMissPolicy::NO_WRITE_ALLOCATE;
    throw runtime_error("Invalid Write Miss Policy" + s + ".\n");
  }
  // lambda conversion map from map<string, string> to map <string, function(string)> converts dynamically based
  // on which config variable is input. This lets our config variables be defined in any order.
  // remove keys on mapping to ensure every value is mapped.

  map<string, function<void(string)>> conversions = {
    // integer config values
    {"_ASSOC", [this](string configValue) { this->associativity = stoi(configValue); }},
    {"_BLOCK_SIZE", [this](string configValue) {this->block_size = stoi(configValue); }},
    {"_SIZE", [this](string configValue) {this->cache_size = stoi(configValue); }},
    {"_HIT_TIME", [this](string configValue) {this->hit_time = stoi(configValue); }},
    // write policy values
    {"_WRITE_HIT", [this](string configValue) {this->write_hit = parseWriteHit(configValue); }},
    {"_WRITE_MISS", [this](string configValue) {this->write_miss = parseWriteMiss(configValue); }},
  };


  cache_config(){};
  cache_config(int cacheSize, int blockSize, int assoc, WriteHitPolicy writeHit, WriteMissPolicy writeMiss) : cache_size(cacheSize),
              block_size(blockSize), associativity(assoc), write_hit(writeHit), write_miss(writeMiss){};
  cache_config(map<string, string> config_map){
    for (auto const& [subkey, subval] : config_map){
      if (conversions.find(subkey) != conversions.end()){
        conversions[subkey](subval);
        conversions.erase(subkey);
      }else{
        cout << "Invalid configuration key: " + subkey << endl;
      }
    }
    if (conversions.size() > 0){
      throw runtime_error("Incomplete cache configuration. Please ensure each cache has 6 correct parameters");
    }
  }
};

#endif