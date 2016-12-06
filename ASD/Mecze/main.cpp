#include<iostream>
#include<algorithm>
using namespace std;

#define change_bit( i, j ) (matches[ (j) ] += powers_of_two[ (i) ])

int main() {

	int n;
	cin >> n;
	int m;
	cin >> m;
	long long int powers_of_two[m];
	long long int power = 1;
	for (int i = 0; i < m; i++) {
		powers_of_two[i] = power;
		power += power;
	}
	long long int matches[n];
	for (int i = 0; i < n; i++)
		matches[i] = 0;
	//tablica matches[i] zawiera informację mówiącą, po której stronie
	//gracz numer i grał w danym meczu
	for (int i = 0; i < m; i++) {
		for (int j = 1; j <= n; j++) {
			int player;
			cin >> player;
			if (j > n/2) {
				//change_bit(i, player - 1);
				matches[player-1] += powers_of_two[i];
			}
		}
	}
	sort(matches, matches + n);
	bool answer = false;
	for (int i = 0; i < n - 1; i++) {
		if (matches[i] == matches[i+1]) {
			answer = true;
			break;
		}
	}
	cout << (answer ? "NIE" : "TAK") << endl;
	return 0;
}
