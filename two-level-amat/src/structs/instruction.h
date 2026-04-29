#ifndef Instruction_H
#define Instruction_H
#include <string>
#include <cstdint>

using namespace std;

struct Instruction{
  string directive{};
  uint32_t address{};
};

#endif