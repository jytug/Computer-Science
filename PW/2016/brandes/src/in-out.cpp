#include "in-out.h"

#include <fstream>
#include <iostream>
#include <string>

digraph<int> read_input(const std::string& filename) {
  std::fstream fs(filename);
  digraph<int> result;
  int v1, v2;

  /* we're assuming the given format here */
  while (fs >> v1 >> v2)
    result.add_edge(v1, v2).add_vertex(v2); // add_vertex is usually ignored
  return std::move(result);
}

void write_output(const std::string& filename,
                  const digraph<int>& network,
                  const std::map<int, double>& result) {
  std::fstream fs(filename);

  /* if the file is not there, create it */
  if (!fs.is_open()) {
    fs.clear();
    fs.open(filename, std::ios::out);
    fs.close();
    fs.open(filename);
  }

  /* we omit vertices with degout(v) = 0 */
  for (const auto& line : result)
    if (network.degout(line.first) > 0)
      fs << line.first << " "
         << line.second << std::endl;
}
