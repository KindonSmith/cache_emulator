#ifndef Lookup_Result_H
#define Lookup_Result_H

#include "cache_structs.h"
using namespace std;

struct lookup_result{
  bool hit;
  bool writeback_occurred;
  cache_block* block;
};


#endif