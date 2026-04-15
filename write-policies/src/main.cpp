using namespace std;
#include "utils/process_input.cpp"
#include "structs/cache_config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]){
  if(argc != 2){
    cout << "Invalid number of arguments. Please enter 1 argument.";
    return 0;
  }

  ifstream fileStream(getFilePath(argv[1]));
  string configString{};

  // open file
  // get 1st line cache config
  // convert 1st line into config data
  // construct cache config
  // build cache based on cache config
  if (fileStream.is_open()){
    // pull config first line into string
    // delimit parse into string array
    // initialize config
    getline(fileStream, configString, '\n');
    Cache myCache = generateCache(configString);
    cout << myCache.block_size << endl;
    
  }else{
    cout << "Input file not found. Please check input file name: " << getFilePath(argv[1]) << endl;
    return 0;
  }
}