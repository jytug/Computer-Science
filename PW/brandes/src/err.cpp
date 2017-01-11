#include "err.h"

#include <iostream>
#include <string>

void usage_err() {
  std::cerr << "Usage: ./brandes proc_num file_in file_out" << std::endl;
  exit(1);
}

void wrong_arg(const std::string &what) {
  std::cerr << "Wrong argument: " << what << std::endl;
  exit(1);
}
