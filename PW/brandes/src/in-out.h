#pragma once

#include "digraph.h"

#include <map>
#include <string>

digraph<int> read_input(const std::string& filename);
void write_output(const std::string& filename,
                  const digraph<int>& network,
                  const std::map<int, double>& result);
