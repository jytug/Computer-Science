/** @file
	Error functions.
*/
#ifndef _MIDDLE_AGES_ERR_H
#define _MIDDLE_AGES_ERR_H

#include <stdio.h>

/** Prints and error message to stderr. */
void input_error() {
	fprintf(stderr, "input error\n");
}

#endif /* _MIDDLE_AGES_ERR_H */
