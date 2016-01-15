#include <iostream>
#include <stdio.h>

using namespace std;

#define MAX 1000000000;

int main() {
	long long int n;
	cin >> n;
	long long int P[n];
	for (long long int i = 0; i < n; i++)
		cin >> P[i];
	
	long long int T0[n][n];
	long long int T1[n][n];
	for (long long int i = 0; i < n; i++) {
		T0[i][i] = 1;
		T1[i][i] = 0;
	}
	for (long long int i = n - 2; i >= 0; i--) {
		for (long long int j = i + 1; j < n; j++) {
			T0[i][j] = (T0[i+1][j] * (P[i] < P[i+1] ? 1 : 0)
				+ T1[i+1][j] * (P[i] < P[j] ? 1 : 0)) % MAX;
	
			T1[i][j] = (T0[i][j-1] * (P[i] < P[j] ? 1 : 0)
				+ T1[i][j-1] * (P[j-1] < P[j] ? 1 : 0)) % MAX;
		}
	}
	long long int res = (T0[0][n-1] + T1[0][n-1]) % MAX;
	printf("%i\n", res);
	return 0;
}
