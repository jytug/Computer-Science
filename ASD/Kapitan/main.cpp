#include <algorithm>
#include <cstdio>
#include <iostream>
#include <queue>
#include <set>
#include <utility>

#define INF 1000000001

using namespace std;

using graph_t = vector<vector<int64_t> >;

class Island {
	private:
		int64_t i;	// number of the island
		int64_t x;	// x coordinate of the island
		int64_t y;	// y coordinate of the island
	public:
		Island() : i(), x(), y() {}
		Island(int64_t i, int64_t x, int64_t y) : i(i), x(x), y(y) {}
		int64_t geti() const { return i; }
		int64_t getx() const { return x; }
		int64_t gety() const { return y; }
};

#ifdef DEBUG
ostream &operator<<(ostream &os, const Island &i0) {
	os << '(' << i0.geti() << ' ' << i0.getx() << ' ' << i0.gety() << ')';
	return os;
}
#endif

bool comp_by_x(const Island &i1, const Island &i2) {
	return i1.getx() < i2.getx();	
}

bool comp_by_y(const Island &i1, const Island &i2) {
	return i1.gety() < i2.gety();
}

int64_t weight(const Island &i1, const Island &i2) {
	return min(abs(i1.getx() - i2.getx()), abs(i1.gety() - i2.gety()));
}

bool comp_rev(int64_t a, int64_t b) {
	
}

int64_t dijkstra(graph_t &g, const vector<Island> &islands) { 	
	/*
	* Unfortunately, the standard template library does not provide 
	* a priority queue template that is efficient in Dijkstra's algorithm.
	* Fortunately, the std::set provides a decent substitute
	*/
	int64_t n = g.size();
	vector<int64_t> distance(n, INF);
	distance[0] = 0;
	set<pair<int64_t, int64_t> > queue;
	for (int64_t i = 0; i < n; ++i)
		queue.insert(make_pair(distance[i], i));
	while (!queue.empty()) {
		int64_t u = queue.begin()->second,
			d = queue.begin()->first;
		queue.erase(queue.begin());
		if (d > distance[u])
			continue;
		for (int64_t i = 0; i < g[u].size(); ++i) {
			int64_t v = g[u][i];
			if (distance[v] > distance[u] + weight(islands[v], islands[u])) {
				pair<int64_t, int64_t> old_pair = make_pair(distance[v], v);
				distance[v] = distance[u] + weight(islands[v], islands[u]);
				auto v_it = queue.find(old_pair);
				queue.erase(v_it);
				queue.insert(make_pair(distance[v], v));			
			}
		}
	}
	return distance[n - 1];
}

int main() {
	int64_t n;
//	cin >> n;
	scanf("%lld", &n);
	vector<Island> islands(n, Island()); 
	for (int64_t i = 0; i < n; ++i) {
		int64_t x, y;
		//cin >> x >> y;
		scanf("%lld %lld", &x, &y);
		islands[i] = Island(i, x, y);
		#ifdef DEBUG
		cout << islands[i].geti() << " "
			<< islands[i].getx() << " "
			<< islands[i].gety() << '\n';
		#endif
	}
	vector<Island> islands_by_x = islands;
	vector<Island> islands_by_y = islands;
	sort(islands_by_x.begin(), islands_by_x.end(), comp_by_x);
	sort(islands_by_y.begin(), islands_by_y.end(), comp_by_y);
	
	#ifdef DEBUG
	cout << "Ordered by x:\n";
	for (int64_t i = 0; i < n; ++i)
		cout << islands_by_x[i] << (i == n - 1 ? '\n' : ' ');

	cout << "Ordered by y:\n";
	for (int64_t i = 0; i < n; ++i)
		cout << islands_by_y[i] << (i == n - 1 ? '\n' : ' ');
	#endif

		/*
		* We construct a graph in which every island is adjacent to at most four
		* other islands: the closest going north, east, south and west (if such 
		* islands exist). We are going to use Dijkstra's algorithm on it
		*/
	graph_t isl_graph(n, vector<int64_t>());
	for (int64_t i = 0; i < n; ++i) {
		if (i > 0) {
			isl_graph[islands_by_x[i].geti()].push_back(islands_by_x[i-1].geti()); // closest west
			isl_graph[islands_by_y[i].geti()].push_back(islands_by_y[i-1].geti()); // closest south
		}
		if (i < n - 1) {
			isl_graph[islands_by_x[i].geti()].push_back(islands_by_x[i+1].geti()); // closest east
			isl_graph[islands_by_y[i].geti()].push_back(islands_by_y[i+1].geti()); // closest north
		}
		sort(isl_graph[i].begin(), isl_graph[i].end());
		auto last = unique(isl_graph[i].begin(), isl_graph[i].end());
		isl_graph[i].erase(last, isl_graph[i].end());
	}
	
	#ifdef DEBUG
	cout << "Graph:\n";
	for (int64_t i = 0; i < n; ++i)
		for (int64_t j = 0; j < isl_graph[i].size(); ++j)
			cout << i << " -> " << isl_graph[i][j] << "\n";
	#endif

	//cout << dijkstra(isl_graph, islands) << '\n'; 
	printf("%lld\n", dijkstra(isl_graph, islands));
	return 0;
}
