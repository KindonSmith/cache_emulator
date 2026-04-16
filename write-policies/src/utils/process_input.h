#ifndef Process_Input_H
#define Process_Input_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include "../structs/cache_config.h"
#include "../structs/cache.h"

using namespace std;

bool validateNumberOfInstructions(int numInstructions){
  if (numInstructions == 2) return true;
  return false;
}
bool validateInstructionReadWrite(string readWrite){
  if (readWrite == "W" || readWrite == "R"){
    return true;
  }
  return false;
}

cache_config buildCacheConfig(vector<string> configArray){
  if(configArray.size() != 5){
    throw runtime_error("Invalid number of arguments for cache configuration. Check the 1st line of your input file.");
  }

  int cacheSize = stoi(configArray[0]);
  int blockSize = stoi(configArray[1]);
  int associativity = stoi(configArray[2]);
  WriteHitPolicy writeHit;
  WriteMissPolicy writeMiss;

  if(configArray[3] == "wb" || configArray[3] == "WB"){
    writeHit = WriteHitPolicy::WRITE_BACK;
  }else if(configArray[3] == "wt" || configArray[3] == "WT"){
    writeHit = WriteHitPolicy::WRITE_THROUGH;
  }else {
    throw runtime_error("Invalid Write Hit policy. Please confirm write policy is the 4th config argument and follows all upper case or all lower-case format. Acceptable arguments: wb | WB | wt | WT");
  }
  if(configArray[4] == "wa" || configArray[4] == "WA"){
    writeMiss = WriteMissPolicy::WRITE_ALLOCATE;
  }else if(configArray[4] == "nwa" || configArray[4] == "NWA"){
    writeMiss = WriteMissPolicy::NO_WRITE_ALLOCATE;
  }else {
    throw runtime_error("Invalid Write Miss policy. Please confirm miss policy is the 5th config argument and follows all upper case or all lower-case format. Acceptable arguments: wa | WA | nwa | NWA");
  }
  if (writeHit == WriteHitPolicy::WRITE_BACK && writeMiss == WriteMissPolicy::NO_WRITE_ALLOCATE){
    throw runtime_error("Invalid configuration. Cannot support write-back hit policy with no-write-allocate miss policy. WB with NWA is not supported.");
  }
  return cache_config(cacheSize, blockSize, associativity, writeHit, writeMiss);
}


Cache generateCache(string configString){
  stringstream ss(configString);
  vector<string> delimitedConfig;
  string token;
  while (ss >> token) {
    delimitedConfig.push_back(token);
  }

  return Cache(buildCacheConfig(delimitedConfig));
  
}

string getFilePath(char* inputFileName){
  return "input/" + (string)inputFileName;
}

void processAddressWithPolicy(string addressWithPolicy, Cache &cache){
  stringstream ss(addressWithPolicy);
  vector<string> instructions{};
  string token{};
  uint32_t address{};

  while (ss >> token) {
    instructions.push_back(token);
  }
  if(!validateInstructionReadWrite(instructions[0])){
    // bad read/write
    return;
  }
  if(!validateNumberOfInstructions(instructions.size())){
    // missing one or both pieces of the instruction
    return;
  }
  address = stoi(instructions[1], 0, 16);
}

void checkInstructionList(ifstream &fileStream, Cache &cache){
  string addressWithPolicy{};
  if (fileStream.is_open()){
    while(getline(fileStream, addressWithPolicy)){
      processAddressWithPolicy(addressWithPolicy, cache);
    }
  }
}





#endif