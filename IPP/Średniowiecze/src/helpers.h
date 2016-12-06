/** @file
	Helper functions.
*/
#ifndef _MIDDLE_AGES_HELPERS_H
#define _MIDDLE_AGES_HELPERS_H

#include <stdlib.h>

/**
	@param[in] x The first number
	@param[in] y The second number
	@return The minimum of x and y
*/
int min(int x, int y) {
	return x < y ? x : y;
}

/**
	@param[in] x The first number
	@param[in] y The second number
	@return The maximum of x and y
*/
int max(int x, int y) {
	return ((x < y) ? y : x);
}

/**
	@param[in] x1 The column of the first point
	@param[in] y1 The row of the first point
	@param[in] x2 The column of the second point
	@param[in] y2 The row of the second point
	@return The maximum metric between the
	points (x1, y1) and (x2, y2)
*/
int max_metric(int x1, int y1, int x2, int y2) {
	return max(abs(x1 - x2), abs(y1 - y2));
}

#endif
