#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>

template <typename T, typename W>
class weighted_graph {
  public:
    using adjacency_list_t = std::unordered_map<T, W>;

    /* constructors */
    weighted_graph() : vertices() {}
    weighted_graph(const weighted_graph& g) = default;
    weighted_graph(weighted_graph&& g) = default;

    /* insertion */
    weighted_graph& add_vertex(const T& v);
    weighted_graph& add_edge(const T& v1, const T& v2, const W& w);

    /* lookup */
    const std::unordered_map<T, adjacency_list_t>& getVertices() const {
        return vertices;
    }

    const adjacency_list_t& getNeighbors(const T& v) const {
        return vertices.at(v);
    }
    const std::size_t deg(const T& v) const {
        return vertices.at(v).size();
    }
    const W weight(const T& v1, const T& v2) const {
        if (v1 == -1 || v2 == -1)
            return 0;

        return vertices.at(v1).at(v2);
    }

    /* visualisation */
    void print();

  private:
    std::unordered_map<T, adjacency_list_t> vertices;
};

template <typename T, typename W>
weighted_graph<T, W>& weighted_graph<T, W>::add_vertex(const T& v) {
    if (vertices.find(v) != vertices.end())
        return *this;
    vertices.emplace(v, weighted_graph::adjacency_list_t());
    return *this;
}

template <typename T, typename W>
weighted_graph<T, W>& weighted_graph<T, W>::add_edge(const T& v1, const T& v2,
                                                     const W& w) {
    /* TODO how much will it slow us down? Perhaps
     * we can create an unsafe version of this function */
    if (vertices.find(v1) == vertices.end())
        this->add_vertex(v1);
    if (vertices.find(v2) == vertices.end())
        this->add_vertex(v2);

    vertices[v1][v2] = w;
    vertices[v2][v1] = w;
    return *this;
}

template <typename T, typename W>
void weighted_graph<T, W>::print() {
    for (const auto& v : vertices) {
        std::cout << "vertex: " << v.first << " (deg = "
            << deg(v.first) << ")" << std::endl << "edges: [ ";
        for (const auto& e : v.second) {
            std::cout << e.first << " ";
        }
        std::cout << "]" << std::endl;
    }
}
