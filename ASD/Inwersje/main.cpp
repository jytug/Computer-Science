//main.cpp

#include <cstdio>
#include <iostream>
#include <algorithm>

#define MAX 1000000000

using namespace std;

int main() {
	long long int n, k;
	scanf("%lld %lld", &n, &k);
	long long int p[n], weights[n];
	for (long long int i = 0; i < n; i++) {
		long long int w;
		scanf("%lld", &w);
		p[i] = w-1;
		//dla wygody obsługujemy
		//permutację liczb {0 .. n-1}
		weights[i] = 1;
	}
	long long int M = 1;
	while (M <= n)
		M *= 2;
		//M = liczba liści w drzewie. Najmniejsza
		//potęga dwójki przekraczająca n
	long long int sum = 0;
	for (long long int j = 2; j <= k; j++) {
		//dla każdego j = 2..k obliczamy
		//wagę[i], równą liczbie j-inwersji kończących
		//się na p[i]
		long long int interval_tree[2*M - 1];
		for (long long int l = 0; l < 2*M - 1; l++)
				interval_tree[l] = 0;
		for (long long int i = 0; i < n; i++) {
			long long int val = p[i] + 1 + M - 1;
				//jesteśmy w liściu o numerze val
			long long int prev_weight = interval_tree[val - 1] = weights[i];
			long long int new_weight = 0;
			long long int current_node = val/2;
			bool coming_from_left = (val % 2 == 0);
			while (current_node > 0) {
				interval_tree[current_node - 1] += prev_weight;	
				if (coming_from_left) {
					new_weight += interval_tree[current_node*2 + 1 - 1] % MAX;
				}
				coming_from_left = (current_node % 2 == 0);
				current_node /= 2;
			}
			weights[i] = new_weight;
			if (j == k)
				sum = ((sum + new_weight) % MAX);
			//cout << "i = " << i << ", new_weight = " << new_weight << endl;
			
		}
		
	}
	printf("%lld", sum);
	return 0;
}
