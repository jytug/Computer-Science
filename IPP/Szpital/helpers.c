#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int equal(char *const s1, char *const s2, int prec) {
	return strncmp(s1, s2, prec) == 0;
}

void ignore() {
	puts("IGNORED");
}

void err() {
	puts("ERROR");
}

void ok() {
	puts("OK");
}

char *copy_string(const char *s) {
	size_t len = strlen(s) + 1;
	char *res = malloc(len*sizeof(char));
	strncpy(res, s, len);
	return res;
}
