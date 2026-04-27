#ifndef Lookup_Result_H
#define Lookup_Result_H

#include "cache_structs.h"
using namespace std;

struct lookup_result{
  bool hit;
  bool writeback_occurred;
  cache_block* victim;

  lookup_result(bool _hit, bool _writeback_occurred, cache_block* _victim) : hit(_hit), writeback_occurred(_writeback_occurred),
                victim(_victim){};
};


#endif