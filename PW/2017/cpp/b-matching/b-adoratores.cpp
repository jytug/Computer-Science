#include "b-adoratores.hpp"
#include "blimit.hpp"

#include <algorithm>
#include <condition_variable>
#include <list>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <unordered_map>

namespace {

using adorators_t = std::unordered_map<
      node_t,
      std::set<std::pair<weight_t, node_t>>
   >;

using adorated_t = std::unordered_map<
      node_t,
      std::unordered_set<node_t>
   >;

node_t get_last(node_t v, adorators_t& S, int b_method) {
    if (S[v].size() == bvalue(b_method, v))
        return S[v].begin()->second;

    return -1;
}

node_t select_adorated(node_t u,
                       const graph_t& graph,
                       adorators_t& S,
                       adorated_t& T,
                       int b_method) {
    // argmax
    node_t result = -1;
    for (auto& _v : graph.getNeighbors(u)) {
        auto v = _v.first;
        if (T[u].find(v) != T[u].end())
            continue;

        auto last = get_last(v, S, b_method);
        if (graph.weight(v, u) > graph.weight(v, last) ||
            (graph.weight(v, u) == graph.weight(v, last) &&
             u > last)) {
            if (graph.weight(u, v) > graph.weight(u, result)) {
                result = v;
            }
        }
    }
    return result;
}

void run_worker(const graph_t& graph,
                int b_method,

                std::queue<node_t>& Q,
                std::queue<node_t>& R,
                adorators_t& S,
                adorated_t& T,
                int& nodes_left,
                std::condition_variable& new_work,
                std::condition_variable& work_done,
                std::mutex& mut) {
    node_t u, x, y;
    for (;;) {
        // wait until there is work to be done
        // and acquire a node to process
        {
            std::unique_lock<std::mutex> lock(mut);
            new_work.wait(lock, [&Q]{ return Q.size() > 0; });
            u = Q.front(); Q.pop();
        }

        // perform the work on u - we're safe now
        while (T[u].size() < bvalue(b_method, u)) {
            x = select_adorated(u, graph, S, T, b_method); 
            if (x == -1) {
                break;
            } else {
                y = S[x].empty() ? -1 : S[x].begin()->second;
                S[x].insert({graph.weight(x, u), u});
                T[u].insert(x);
                if (y != -1) {
                    T[y].erase(x);
                    R.push(y);
                }
            }
        }
        
        // notify the main thread that work might be done
        std::unique_lock<std::mutex> lock(mut);
        nodes_left--;
        work_done.notify_one();
    }
}

long b_adoratores_parallel(const graph_t& graph,
                           int num_threads,
                           int b_method) {
    std::queue<node_t> Q;
    std::queue<node_t> R;
    adorators_t S;
    adorated_t T;
    int nodes_left;
    std::condition_variable new_work;
    std::condition_variable work_done;
    std::mutex mut;

    for (auto& _v : graph.getVertices()) {
        auto v = _v.first;
        Q.push(v);
        S[v] = std::set<std::pair<weight_t, node_t>>();
        T[v] = std::unordered_set<node_t>();
    }

    nodes_left = Q.size();

    std::vector<std::thread *> workers(num_threads - 1);

    for (std::size_t i = 0; i < num_threads - 1; i++) {
        workers[i] = new std::thread{
            [&, b_method] {
                 run_worker(graph, b_method,
                            Q, R, S, T, nodes_left,
                            new_work, work_done, mut
                         );
        }}; 
    }

    {
        std::unique_lock<std::mutex> lock(mut);
        while (!Q.empty()) {
            work_done.wait(lock, [&]{ return nodes_left == 0; });
            std::queue<node_t> dummy;
            std::swap(Q, R);
            std::swap(R, dummy);
            new_work.notify_all();
        }
    }
    for (std::size_t i = 0; i < num_threads - 1; i++) {
        delete workers[i];
    }

    return 0; 
}

} // anonymous namespace

std::list<long>
b_adoratores_parallel_range(const graph_t& graph,
                            int num_threads,
                            int b_limit) {
    std::list<long> result;
    for (int b_method = 0; b_method <= b_limit; b_method++) {
        result.push_back(
            b_adoratores_parallel(graph, num_threads, b_method)
        );
    }
    return result; 
}
