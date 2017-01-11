#pragma once

#include "digraph.h"

#include <map>

std::map<int, double>
parallel_brandes(const digraph<int>& network, int num_threads);
