#ifndef Stats_H
#define Stats_H


struct Stats{
  int total_accesses{};
  int reads{};
  int writes{};
  int hits{};
  int misses{};
  int writebacks{};
  int writes_to_next_level{};
};

#endif
