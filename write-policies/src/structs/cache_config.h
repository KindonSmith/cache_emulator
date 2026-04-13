
#ifndef Cache_Config_H
#define Cache_Config_H


enum class WriteHitPolicy {WRITE_BACK, WRITE_THROUGH};
enum class WriteMissPolicy {WRITE_ALLOCATE, NO_WRITE_ALLOCATE};

struct cache_config{
  int cache_size{};
  int block_size{};
  int associativity{};
  WriteHitPolicy write_hit{};
  WriteMissPolicy write_miss{};

  
};

#endif