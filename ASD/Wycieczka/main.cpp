#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <vector>

#define INF 10000000

using namespace std;

int main() {
	int n, k;
	scanf("%d %d", &n, &k);
	k = min(n+1, k);
	int first_taxi;
	scanf("%d", &first_taxi);

	//int prev_city[2][k+1][n/2 + 1];	
	//int current_city[2][k+1][n/2 + 1];
	vector<vector<int> > prev_city(
		k+1, vector<int>(n/2 + 1, INF));

	vector<vector<int> > current_city(
		k+1, vector<int>(n/2 + 1, INF));



	/*
		tutaj trzymam informację, ile mógł kosztować transport
		do poprzedniego miasta. prev_city[0] dotyczy dworca,
		prev_city[1] - lotniska. Drugi argument mówi, jaka była dotąd
		liczba przejazdów taksówką, trzeci - jaka była liczba przejazdów
		koleją.

		Wtedy current_city(0,j,l) = 
			min(prev_city(0,j,l-1) + koszt pociągu,
				prev_city(1,j-1,l) + koszt samolotu + koszt taksówki

		current_city(1,j,l) = 
			min(prev_city(1,j,l) + koszt samolotu,
				prev_city(0,j-1,l-1) + koszt pociągu + koszt taksówki

		UWAGA: żeby zaoszczędzić na czasie, informację, czy jesteśmy 
		na dworcu, czy na lotnisku można wyjąć z liczby wykorzystanych
		taksówek zatem
			prev_city(j,l) oznacza lotnisko, jeśli j = 1 (mod 2)
			i stację kolejową w przeciwnym wypadku
	*/
	prev_city[0][0] = 0;
	prev_city[1][0] = first_taxi;

	for (int i = 1; i <= n; i++) {
		int rail, taxi, air;
		scanf("%d %d %d", &rail, &taxi, &air);
		//jesteśmy w mieście numer i
		for (int j = 0; j <= k; j++) {
			for (int l = 0; l <= n/2; l++) {
				int cost1, cost2, cost3, cost4;
				if (j % 2 == 0) {
					cost1 = (l > 0 ? prev_city[j][l-1] + rail : INF);
					cost2 = (j > 0 ? prev_city[j-1][l] + air + taxi : INF);
					current_city[j][l] = min(cost1, cost2);
				} else {
					cost3 = prev_city[j][l] + air;
					cost4 = ((j > 0 && l > 0) ? prev_city[j-1][l-1]
						+ rail + taxi : INF);
					current_city[j][l] = min(cost3, cost4);

				}
			}
		}
		prev_city = current_city;
	}
	int min_cost = INF;
	for (int i = 1; i <= k; i += 2)
		min_cost = min(min_cost, current_city[i][n/2]);
	printf("%d\n", min_cost);
	return 0;
}
