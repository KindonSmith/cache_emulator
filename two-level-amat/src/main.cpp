#include "classes/cache.h"
#include "utils/input_parsing.h"
#include "utils/build_cache.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string> 

using namespace std;

int main (int argc, char** argv){
  // ./cache_sim --config configs/default.cfg input/trace1.txt
  // need a function to find config file in args. I don't want to hardcode "argv[1] = this arg" again
  // so we can use a function to check for config file and increment index


  ifstream configFile = getConfigFile(argc, argv);
  string configLine{};
  if (configFile.is_open()){
    // process config file
    // do we want to get 2 cache_configs? do we just build cache_set?
    parseConfig(configFile);

  } else {
    __throw_runtime_error("Unable to open config file.");
  }
  
}