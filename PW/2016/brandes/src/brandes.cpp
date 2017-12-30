#include "brandes-parallel.h"
#include "brandes-sequential.h"
#include "err.h"
#include "in-out.h"

#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 4)
    usage_err();

  int num_threads = atoi(argv[1]);
  if (num_threads < 1)
    wrong_arg("num_threads non-positive or NaN");

  std::string file_in = std::string(argv[2]);
  std::string file_out = std::string(argv[3]);

  digraph<int> network = read_input(file_in);

  std::map<int, double> result =
    num_threads == 1 ? 
    sequential_brandes(network) :
    parallel_brandes(network, num_threads);


  write_output(file_out, network, result);
  return 0;
}
