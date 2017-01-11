#include "brandes-sequential.h"

#include <queue>
#include <list>
#include <stack>
#include <unordered_map>

/*
 * This is an implementation of one-threaded Brandes algorithm
 * and it has been implemented for my betted understanding
 * of the algorithm and performance / corectness reference
 */
std::map<int, double>
sequential_brandes(const digraph<int>& network) {
  std::map<int, double> betweenness;
  for (const auto& _v : network.getVertices())
    betweenness[_v.first] = .0;

  for (const auto& _s : network.getVertices()) {
    int s = _s.first;
    std::stack<int> S;    
    std::queue<int> Q;
    std::unordered_map<int, std::list<int> > predecessors;
    std::unordered_map<int, int> shortest_paths;
    std::unordered_map<int, int> distance;
    std::unordered_map<int, int> delta;

    for (const auto& _w : network.getVertices()) {
      int w = _w.first;
      predecessors[w] = std::list<int>();
      shortest_paths[w] = 0;
      distance[w] = -1;
      delta[w] = 0;
    }

    shortest_paths[s] = 1;
    distance[s] = 0;
    Q.push(s);
    while (!Q.empty()) {
      int v = Q.front(); Q.pop();
      S.push(v);
      for (const int w : network.getNeighbors(v)) {
        if (distance[w] < 0) {
          Q.push(w);
          distance[w] = distance[v] + 1;
        }
        if (distance[w] == distance[v] + 1) {
          shortest_paths[w] += shortest_paths[v];
          predecessors[w].push_back(v);
        }
      }
    }
    while (!S.empty()) {
      int w = S.top(); S.pop();
      for (const auto& v : predecessors[w])
        delta[v] += (shortest_paths[v] / shortest_paths[w]) * (1 + delta[w]);
      if (w != s)
        betweenness[w] += delta[w];
    }
  }
  return std::move(betweenness);
}
