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

int getConfigFileArgumentIndex(int numArgs, char** argv){
  
  // goes from 0 to args[numArgs-2], because if last arg (args[numArgs-1]) 
  // is --config, we know there is no config file after
  for(int i = 0; i < numArgs - 1; i++){
    if (string(argv[i]) == "--config"){
      return i+1;
    }
  }
  // if we reach here, no config file found. we can't proceed without a config file so error out
  throw runtime_error("No configuration file. Please include --config [config.txt]");
  // extra safety, shouldn't hit.
  return 0;
}

string getConfigFileString(int numArgs, char** argv){
  return (string)argv[getConfigFileArgumentIndex(numArgs, argv)];
}

ifstream getConfigFile(int numArgs, char** argv){
  ifstream inputFile(getConfigFileString(numArgs, argv));
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
  pandv[configLine.substr(0, positionIdent)].insert({configLine.substr(positionIdent, positionEquals - 2), configLine.substr(positionEquals+1)});
}

void insertCachesIntoSystem(const map<string, map<string, string>> pandv, Cache_System &system){
  for (auto const& [key, val] : pandv){
    // for each cache (l1, l2, etc.) build a config from it nad throw it into our cache system by ref
    if (key != "MEMORY"){
      buildCacheConfig(val, system);
    }else {
      // memory access time handle
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



#endif