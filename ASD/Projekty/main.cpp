#include <algorithm>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

int main() {
	int n, m, k;
	cin >> n >> m >> k;
	vector<int> cost(n, 0);
	for (int i = 0; i < n; i++) {
		cin >> cost[i];
	}
	vector<vector<int> > dag(n, vector<int>());	
	vector<int> degin(n, 0);
	for (int i = 0; i < m; i++) {
		int pred, succ;
			//successor, predecessor
		cin >> succ >> pred;
		succ--; pred--;
		dag[pred].push_back(succ);
		degin[succ]++;	
}
	//Now we have the representation od the DAG
	//and an array of in-degrees
	
	multimap <int, int> queue;
	//we insert a pair: cost, number of node
	//into a multimap
	for (int i = 0; i < n; i++) {
		if (degin[i] == 0)
			queue.insert(pair<int, int>(cost[i], i));
	}

	int result = 0;	
	for (int i = 0; i < k; i++) {
		auto curr_min = queue.begin();
		if (curr_min != queue.end()) {
			result = max(result, curr_min->first);
			for (int j = 0; j < dag[curr_min->second].size(); j++) {
				int curr_succ = dag[curr_min->second][j];
				if (--degin[curr_succ] == 0)
					queue.insert(pair<int,int>(cost[curr_succ], curr_succ));
			}
			queue.erase(curr_min);
		} else
			break;
	}
	cout << result << '\n';
	return 0;
}
