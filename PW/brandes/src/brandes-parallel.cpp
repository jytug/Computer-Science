#include "brandes-parallel.h"

#include <chrono>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <thread>
#include <vector>

void count_betweenness(const digraph<int>& network,
                       std::mutex& mut,
                       std::map<int, double>& betweenness,
                       std::stack<int>& to_go) {
  int s;
  for (;;) {
    mut.lock();
    if (to_go.empty()) {
      mut.unlock();
      break;
    }
    s = to_go.top(); to_go.pop();
    mut.unlock();

    /* do the algorithm on v */
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
      if (w != s) {
        mut.lock();
        betweenness[w] += delta[w];
        mut.unlock();
      }
    }
    /* algorithm is over */
  }
}

/* it creates num_threads threads to perfom the Brandes algorithm */
std::map<int, double>
parallel_brandes(const digraph<int>& network, int num_threads) {
  /* the mutex is used to synchronize access to shared data */
  std::mutex mut;
  /* the return value */
  std::map<int, double> betweenness;
  /* contains all the vertices yet to be processed */
  std::stack<int> to_go; 

  for (const auto& _v : network.getVertices()) {
    betweenness[_v.first] = .0;
    to_go.push(_v.first);
  }
  std::vector<std::thread> threads(num_threads);
  for (int i = 0; i < num_threads; ++i)
    threads[i] = std::thread{[&network, &mut, &betweenness, &to_go] {
      count_betweenness(network, mut, betweenness, to_go);
    }};
  for (int i = 0; i < num_threads; ++i)
    threads[i].join();
  return std::move(betweenness);
}
