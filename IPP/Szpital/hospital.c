#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "helpers.h"
#include "parse.h"

#define ARG "-v"
#define ARG_LEN 3

int main(int argc, char *argv[]) {
	int err_info = 0;
	if (argc == 2 && strncmp(argv[1], ARG, ARG_LEN) == 0) {
		err_info = 1;
	} else if (argc == 1) {
		err_info = 0;
	} else {
		err();
		return 1;
	}
	do_the_reading(err_info);
	return 0;
}
