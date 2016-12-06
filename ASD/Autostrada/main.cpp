#include <iostream>
#include <iterator>
#include <cstdio>
#include <utility>
#include <map>
#include <algorithm>

using namespace std;

typedef pair<int64_t, int64_t> interval;
typedef map<int64_t, int64_t> highway;
	//w obiekcie typu highway będziemy trzymać
	//pary stanowiące wartość lewego i prawego końca
	//każdego białego odcinka autostrady.
	//Cała reszta jest czarna

bool is_subinterval(int64_t a, int64_t b, int64_t c, int64_t d) {
	//zwraca true, jeśli i1 \subset i2
	return (a >= c && b <= d);
}

bool intersect(int64_t a, int64_t b, int64_t c, int64_t d) {
	return (!((b < c) || d < a));
}

int64_t add_white(highway &h, const int64_t first, const int64_t second) {
	int64_t result = 0;
	highway::iterator it = h.lower_bound(first);
	if (it != h.begin()) {
		--it;
		if (!intersect(it->first, it->second, first, second))
			++it;
	}
	int64_t thrown_away_km = 0;
	int64_t to_add_first = first, to_add_second = second;
	highway::iterator to_throw_away_first = it, to_throw_away_second = it;

	while (it != h.end() && intersect(it->first, it->second, first, second)) {
			thrown_away_km += (it->second - it->first) + 1;
			to_add_first = min(it->first, to_add_first);
			to_add_second = max(it->second, to_add_second);
			it++;
	}	
	to_throw_away_second = it;
	if (to_throw_away_first != to_throw_away_second) {
		h.erase(to_throw_away_first, to_throw_away_second);
	}
	h.insert(make_pair(to_add_first, to_add_second));

	result = (to_add_second - to_add_first + 1) - thrown_away_km;
	return result;	
}

int64_t add_black(highway &h, const int64_t first, const int64_t second) {
	int64_t result = 0;
	highway::iterator it = h.lower_bound(first);
	if (it != h.begin()) {
		--it;
		if (!intersect(it->first, it->second, first, second))
			++it;
	}
	int64_t thrown_away_km = 0;
	int64_t to_add_first_1 = 0, to_add_second_1 = 0;
	int64_t to_add_first_2 = 0, to_add_second_2 = 0;
	highway::iterator to_throw_away_first = it, to_throw_away_second = it;

	while (it != h.end() && intersect(it->first, it->second, first, second)) {
			thrown_away_km += it->second - it->first + 1;
			if (!is_subinterval(it->first, it->second, first, second)) {
				if (it->first < first) {
					//biały odcinek wystaje **z lewej strony**
					to_add_first_1 = it->first;
					to_add_second_1 = first - 1;
				}
				if (it->second > second) {
					//biały odcinek wystaje **z prawej strony**
					to_add_first_2 = second + 1;
					to_add_second_2 = it->second;
				}
			}	
			it++;
	}	
	to_throw_away_second = it;
	if (to_throw_away_first != to_throw_away_second) {
		h.erase(to_throw_away_first, to_throw_away_second);
	}
	if (to_add_first_1 != 0) {
		h.insert(make_pair(to_add_first_1, to_add_second_1));
		result += to_add_second_1 - to_add_first_1 + 1;
	}
	if (to_add_first_2 != 0) {
		h.insert(make_pair(to_add_first_2, to_add_second_2));
		result += to_add_second_2 - to_add_first_2 + 1;
	}
	result -= thrown_away_km;
	return result;	
}
int main() {
	int64_t n, m;
//	scanf("%ld%ld", &n, &m);
	cin >> n;
	cin >> m;
	int64_t white_kilometers = 0;
	highway h;
	for (int64_t i = 0; i < m; i++) {
		int64_t left, right;
		char bow; //black or white
		//scanf("%ld %ld %c", &left, &right, &bow);
		cin >> left >> right >> bow;
		if (bow == 'B') {
			white_kilometers += add_white(h, left, right);
		} else if (bow == 'C') {
			white_kilometers += add_black(h, left, right);
		}
		//printf("%ld\n", white_kilometers);
		cout << white_kilometers << endl;
	}
	return 0;
}
