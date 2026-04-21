#ifndef Input_Parsing_H
#define Input_Parsing_H
#include <cstdint>
#include <iostream>

int getConfigFileArgumentIndex(int numArgs, char** args){
  
  // goes from 0 to args[numArgs-2], because if last arg (args[numArgs-1]) 
  // is --config, we know there is no config file after
  for(int i = 0; i < numArgs - 1; i++){
    cout << args[i] << endl;
    if (string(args[i]) == "--config"){
      return i+1;
    }
  }
  throw std::runtime_error("No configuration file. Please include --config [config.txt]");
  return 0;
}

#endif