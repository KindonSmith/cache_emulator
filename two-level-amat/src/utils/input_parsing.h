#ifndef Input_Parsing_H
#define Input_Parsing_H
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
  __throw_runtime_error("No configuration file. Please include --config [config.txt]");
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

map<string, string> splitCacheConfigParameterAndValue(string configLine){
  size_t positionEquals = configLine.find('=');
  size_t positionIdent = configLine.find('_');
  map<string, string> configValue{};
  configValue.insert({configLine.substr(positionIdent, positionEquals), configLine.substr(positionEquals+1)});
  return configValue;
}

void parseConfig(ifstream &configFile){
  string configLine{};

  while(getline(configFile, configLine)){
    splitCacheConfigParameterAndValue(configLine);
  } 


}



#endif