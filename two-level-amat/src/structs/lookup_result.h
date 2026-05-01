#ifndef Lookup_Result_H
#define Lookup_Result_H

#include "cache_structs.h"
using namespace std;

struct lookup_result{
  bool hit;
  bool writeback_occurred;
  uint32_t address;
  uint32_t wb_victim_address;

  lookup_result(bool _hit, bool _writeback_occurred, cache_block _victim, 
                uint32_t _address, uint32_t _victim_address) : 
                hit(_hit), 
                writeback_occurred(_writeback_occurred), 
                address(_address),
                wb_victim_address(_victim_address){};
};


#endif