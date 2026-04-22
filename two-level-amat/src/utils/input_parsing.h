#ifndef Input_Parsing_H
#define Input_Parsing_H
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int getConfigFileArgumentIndex(int numArgs, char** argv){
  
  // goes from 0 to args[numArgs-2], because if last arg (args[numArgs-1]) 
  // is --config, we know there is no config file after
  for(int i = 0; i < numArgs - 1; i++){
    if (string(argv[i]) == "--config"){
      return i+1;
    }
  }
  throw std::runtime_error("No configuration file. Please include --config [config.txt]");
  return 0;
}

string getConfigFileString(int numArgs, char** argv){
  return (string)argv[getConfigFileArgumentIndex(numArgs, argv)];
}

ifstream getConfigFile(string filePath){
  ifstream inputFile(filePath);
  return inputFile;
}

#endif