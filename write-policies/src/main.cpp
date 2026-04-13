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
  string config{};
  if (fileStream.is_open()){
    getline(fileStream, config, '\n');
  }else{
    cout << "Input file not found. Please check input file name" << endl;
    return 0;
  }
}