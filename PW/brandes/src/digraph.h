#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

template <typename T>
class digraph {
public:
  using adjacency_list_t = std::unordered_set<T>;

  /* constructors */
  digraph() : vertices() {};
  digraph(const digraph& d) = default;
  digraph(digraph&& d) = default;

  /* modifying methods */
  digraph& add_vertex(const T& v);
  digraph& add_vertex(const T& v, adjacency_list_t&& neighbors);
  digraph& add_edge(const T& v1, const T& v2);

  /* access methods */
  const std::unordered_map<T, adjacency_list_t>& getVertices() const {
    return vertices;
  }

  const adjacency_list_t& getNeighbors(const T& v) const { return vertices.at(v); }
  const std::size_t degout(const T& v) const {
    return vertices.at(v).size();
  }

  /* debugging helpers */
  void output();
private:
  std::unordered_map<T, adjacency_list_t> vertices;
};

template<typename T>
digraph<T>& digraph<T>::add_vertex(const T& v) {
  if (vertices.find(v) != vertices.end())
    return *this;
  vertices.emplace(v, digraph::adjacency_list_t());
  return *this;
}

template <typename T>
digraph<T>& digraph<T>::add_vertex(const T& v,
                                   digraph::adjacency_list_t&& neighbors) {
  vertices.emplace(v, neighbors);
  return *this;
}

template <typename T>
digraph<T>& digraph<T>::add_edge(const T& v1, const T& v2) {
  vertices[v1].insert(v2);
  return *this;
}

template <typename T>
void digraph<T>::output() {
  for (const auto& v : vertices) {
    std::cout << "vertex: " << v.first << " (degout = "
              << degout(v.first) << ")" << std::endl << "edges: [ ";
    for (const auto& e : v.second) {
      std::cout << e << " ";
    }
    std::cout << "]" << std::endl;
  }
}
