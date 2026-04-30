#ifndef Input_Parsing_H
#define Input_Parsing_H
#include "../classes/cache_system.h"
#include "build_cache.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;


int getFileArgumentIndex(int numArgs, char** argv, string file_flag){
  
  // goes from 0 to args[numArgs-2], because if last arg (args[numArgs-1]) 
  // is --config, we know there is no config file after
  for(int i = 0; i < numArgs - 1; i++){
    if (string(argv[i]) == file_flag){
      return i+1;
    }
  }
  // if we reach here, no file found. we can't proceed without proper files so error out
  throw runtime_error("File not found:" + file_flag);
  // extra safety, shouldn't hit.
  return 0;
}

string getFileString(int numArgs, char** argv, string file_flag){
  return (string)argv[getFileArgumentIndex(numArgs, argv, file_flag)];
}

ifstream getFile(int numArgs, char** argv, string file_flag){
  ifstream inputFile(getFileString(numArgs, argv, file_flag));
  return inputFile;
}

void mapCacheConfigParameterToValue(string configLine, map<string, map<string, string>> &pandv){
  long long positionEquals = configLine.find('=');
  long long positionIdent = configLine.find('_');
  // need to run checks to make sure our config input is working as expected.
  if (positionEquals == string::npos || positionIdent == string::npos) { 
    throw runtime_error("Malformed config input." + configLine);
  }
  if (positionEquals == 0 || positionEquals == 1){
    throw runtime_error("Malformed config input: " + configLine + ". \n" + "Please include a parameter before equal sign.");
  }
  if (positionEquals == configLine.size() - 1)
  {
    throw runtime_error("Malformed config input: " + configLine + ". \n" + "Please include a value after the equal sign.");
  }
  pandv[configLine.substr(0, positionIdent)].insert({configLine.substr(positionIdent, positionEquals - positionIdent), configLine.substr(positionEquals+1)});
}

void insertCachesIntoSystem(const map<string, map<string, string>> pandv, Cache_System &system){
  for (auto const& [key, val] : pandv){
    // for each cache (l1, l2, etc.) build a config from it nad throw it into our cache system by ref
    if (key != "MEMORY"){
      buildCacheConfig(val, system);
    }else {
      // memory access time handle
      system.set_access_time(stoi(val.at("_ACCESS_TIME")));
    }
  }   
}

void parseConfig(ifstream &configFile, Cache_System &system){
  string configLine{};
  map<string, map<string, string>> pandv{};
  
  if (!configFile.is_open()) {
    throw runtime_error("Unable to open configuration file. Please verify config filepath.");
  }

  while(getline(configFile, configLine)){
    mapCacheConfigParameterToValue(configLine, pandv);
  } 
  insertCachesIntoSystem(pandv, system);

}

bool validateNumberOfInstructions(int numInstructions){
  if (numInstructions == 2) return true;
  cout << "Skipped invalid instruction. Too many line items;" << endl;

  return false;
}
bool validateInstructionReadWrite(string readWrite){
  if (readWrite == "W" || readWrite == "R"){
    return true;
  }
  cout << "Skipped invalid instruction: " << readWrite << ". Use R or W." << endl;
  return false;
}

bool processAddressWithPolicy(string addressWithPolicy, Instruction &out){
  stringstream ss(addressWithPolicy);
  vector<string> instruction{};
  string token{};
  uint32_t address{};

  while (ss >> token) {
    instruction.push_back(token);
  }
  if(!validateInstructionReadWrite(instruction[0])){
    // bad read/write
    return false;
  }
  if(!validateNumberOfInstructions(instruction.size())){
    // missing one or both pieces of the instruction
    return false;
  }
  out.directive = instruction[0];
  out.address = stoi(instruction[1], 0, 16);
  return true;
  // address = stoi(instruction[1], 0, 16);
  //cache.lookUp(address, instruction[0]);
}

void parseInstructions(ifstream &fileStream, Cache_System& system){
  string addressWithPolicy{};
  Instruction inst;
  if (fileStream.is_open()){
    while(getline(fileStream, addressWithPolicy)){
      if (processAddressWithPolicy(addressWithPolicy, inst)){
        system.insert_instruction(inst);
      }
    }
  }else{
    throw runtime_error("Input File not found");
  }
}



#endif