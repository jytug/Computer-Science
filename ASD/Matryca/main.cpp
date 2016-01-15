#include<iostream>
#include<string.h>
#include<stdio.h>

using namespace std;

int main() {
	string pattern;
	cin >> pattern;
	long long int max = 1000000;
	long long int min_stars = max + 1;
		//min_stars oznacza najmniejszą liczbę gwiazdek
		//oddzielających dwa różne kolory
		//odpowiedzią jest długość pattern - min_stars
	long long int left, right;
	long long int length = pattern.length();
	if (length == 1) {
		printf("1\n");
	} else {
		left = 0;
		right = 1;
		while (right < length && min_stars > 0) {
			if (pattern[left] == pattern[right]) {
				left++;
				right++;
			} else {
				if (pattern[left] == '*') {
					left = right++;
				} else {
					if (pattern[right] == '*') {
						right++;
					} else {
						//pattern[left] != pattern[right]
						//oraz obydwa są rózne od *
						if (right - left - 1 < min_stars)
							min_stars = right - left - 1;
						left = right++;
					}
				}
			}
		}
		if (min_stars == max + 1)
			printf("1\n");
		else {
			printf("%lld\n", length - min_stars);
		}
	}
	return 0;
}
