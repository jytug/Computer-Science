#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ignore() {
	puts("ignored");
};

char *copy_string(const char *s) {
	size_t len = strlen(s) + 1;
	char *res = malloc(len*sizeof(char));
	strncpy(res, s, len);
	return res;
}
