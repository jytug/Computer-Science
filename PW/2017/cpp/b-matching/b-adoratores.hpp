#pragma once

#include "config.hpp"
#include "graph.hpp"

#include <list>

std::list<long>
b_adoratores_parallel_range(const graph_t& graph,
                            int num_threads,
                            int b_limit);
