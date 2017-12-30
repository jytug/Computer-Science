#include <iostream>
#include <fstream>
#include <sstream>

#include "config.hpp"
#include "graph.hpp"
#include "b-adoratores.hpp"

void usage_err() {
    std::cerr << "Usage: adore <num_threads> <graph_file> <b_limit>"
              << std::endl;
    exit(1);
}

void bad_file() {
    std::cerr << "Graph file is not properly formatted"
              << std::endl;
    exit(2);
}

bool is_comment(const std::string& line) {
    return line.size() == 0 || line[0] == '#';
}

int main(int argc, char *argv[]) {
    long num_threads, b_limit;
    std::string graph_file;

    if (argc != 4) {
        usage_err();
    }

    try {
        num_threads = std::stol(argv[1]);
        graph_file = std::string(argv[2]);
        b_limit = std::stol(argv[3]);
    } catch (...) {
        usage_err();
    }

    graph_t graph;
    std::ifstream infile(graph_file);
    std::string line;

    while (std::getline(infile, line)) {
        if (is_comment(line))
            continue;

        node_t v1, v2;
        weight_t w;
        std::istringstream iss(line);
        if (!(iss >> v1 >> v2 >> w))
            bad_file();

        graph.add_edge(v1, v2, w);
    }

    auto result = b_adoratores_parallel_range(
            graph,
            num_threads,
            b_limit);

    for (auto i : result) {
        std::cout << i << std::endl;
    }

}
