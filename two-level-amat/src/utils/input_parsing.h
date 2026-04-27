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

void splitCacheConfigParameterAndValue(string configLine, map<string, map<string, string>> &pandv){
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

void parseConfig(ifstream &configFile, Cache_System &system){
  string configLine{};
  map<string, map<string, string>> pandv{};
  
  if (!configFile.is_open()) {
    throw runtime_error("Unable to open configuration file. Please verify config filepath.");
  }

  while(getline(configFile, configLine)){
    splitCacheConfigParameterAndValue(configLine, pandv);
  } 

  
  /* debug
  for (const auto& cacheConfig : pandv){
    for (const auto& param : cacheConfig.second){
      cout << cacheConfig.first << " | " << param.first << " | " << param.second << endl;
    }
  }
  */
  for (auto const& [key, val] : pandv){
    // for each cache (l1, l2, etc.) build a config from it nad throw it into our cache system by ref
    buildCacheConfig(val, system);
    cout << system.get_cache_list().size() << endl;
    /*
    for (auto const& [subkey, subval] : val){
      cout << subkey 
           << subval
           << endl;
    }*/
  }      

}



#endif