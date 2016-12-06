#include <iostream>
#include <cstdio>

using namespace std;

int main() {
	long long int n;
	scanf("%lld", &n);
	long long int prices[n];
	for (long long int i = 0; i < n; i++)
		scanf("%lld", &prices[i]);
		//cin >> prices[i];
		//tablica prices jest posortowana niemalejąco
	long long int suffix_sums[n];
		//suffix_sums[i] = suma prices od i do n-1 
	for (long long int i = n-1; i >= 0; i--) {
		if (i == n-1)
			suffix_sums[i] = prices[i];
		else
			suffix_sums[i] = prices[i] + suffix_sums[i+1];
	}
	long long int next_odd[n], next_even[n], prev_odd[n], prev_even[n];
		/* next_odd[i] = najmniejsze j > i takie, 
			że prices[j] jest liczbą nieparzystą (albo -1).
		prev_odd[i] = największe j <= i takie,
			że prices[j] jest liczbą nieparzystą (albo -1).
		next_even, prev_even są odpowiednikami dla liczb parzystych
		*/
	long long int nodd = -1, neven = -1, podd = -1, peven = -1;
	for (long long int i = 0; i < n; i++) {

		if (prices[i] % 2 == 1)
			podd = i;
		else
			peven = i;

		next_odd[n-i-1] = nodd;
		next_even[n-i-1] = neven;
		prev_odd[i] = podd;
		prev_even[i] = peven;

		if (prices[n-i-1] % 2 == 1)
			nodd = n-i-1;
		else
			neven = n-i-1; 
	}


	long long int m;
	scanf("%lld", &m);
	//cin >> m;
	for (long long int i = 0; i < m; i++) {
		long long int k;
		scanf("%lld", &k);
		//cin >> k;
			//szukamy podciągu ciągu prices[] długości k o największej
			//nieparzystej sumie
		if (k <= n) {
			long long int sum = suffix_sums[n - k];
			if (sum % 2 == 1) {
				printf("%lld\n", sum);
				//cout << sum << endl;
			} else {
				long long int pos = n - k - 1;
				if (pos == -1) {
					printf("-1\n");
					//cout << -1 << endl;
				} else {
					long long int l1 = prev_odd[pos],
						l2 = prev_even[pos],
						r1 = next_even[pos],
						r2 = next_odd[pos];
					long long int minimal_difference = 0;
					if (l1 != -1 && r1 != -1) {
						minimal_difference = prices[r1] - prices[l1];
					}
					if (l2 != -1 && r2 != -1) {
						if (minimal_difference > 0) { 
							if (minimal_difference > prices[r2] - prices[l2])
								minimal_difference = prices[r2] - prices[l2];
						} else {
							minimal_difference = prices[r2] - prices[l2];
						}
					}
					if (minimal_difference > 0)
						printf("%lld\n", sum - minimal_difference);
						//cout << sum - minimal_difference << endl;
					else
						printf("-1\n");
						//cout << -1 << endl;
				}
			}
		} else {
			printf("-1\n");
			//cout << -1 << endl;
		}
	}
	return 0;
}
