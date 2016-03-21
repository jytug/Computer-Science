#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	char * line = 0;
	size_t nbytes = 0;
	while (getline(&line, &nbytes, stdin) != -1) {
		printf("%s", line);
	} 
	free(line);
	return 0;
}
