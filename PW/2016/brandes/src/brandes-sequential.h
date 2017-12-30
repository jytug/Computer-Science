#pragma once

#include <map>

#include "digraph.h"

std::map<int, double>
sequential_brandes(const digraph<int>& network);
