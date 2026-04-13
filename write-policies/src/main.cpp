using namespace std;
#include "utils/process_input.cpp"
#include "structs/cache_config.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]){
  if(argc != 2){
    cout << "Invalid number of arguments. Please enter 1 argument."
    return 0;
  }

  // arg should be filename, which will hold config and data.
  string getFilePath = process_input(argv[1]);



}