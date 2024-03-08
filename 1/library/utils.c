#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "functions.h"

bool cas(double *number, double old, double new_one) {
	if (*number != old)
		return false;
	*number = new_one;
	return true;
}

double randfrom(double min, double max) {
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

int min(int x, int y) {
	return (x < y) ? x : y;
}

int error_msg(char* msg, int err) {
	fprintf(stderr, "%s", msg);
	return err;
}

void matrix_free(double **u, size_t size) {
	for (size_t i = 0; i < size; i++)
		free(u[i]);
	free(u);
}

double **matrix_malloc(size_t size) {
	double **u = (double **) malloc(sizeof(double *) * (size));
	for (size_t i = 0; i < size; i++)
		u[i] = (double *) malloc(sizeof(double) * (size));
	return u;
}

void matrix_init(double **u, grid_t grid, double min_border, double max_border) {
	double x, y;
	srand(SEED);

	for (size_t i = 0; i < grid.grid_size + 2; i++) {
		for (size_t j = 0; j < grid.grid_size + 2; j++) {
			x = i * grid.h; y = j * grid.h;
			if (x == 0 || x == 1 || y == 0 || y == 1) u[i][j] = g(x, y);
			else u[i][j] = randfrom(min_border, max_border);
		}
	}

}

int arg_parse(int argc, char **argv, size_t *grid_size, double *eps, double *rand_min_border, double *rand_max_border) {

	if (argc > MAX_NUM_OF_ARGS) {
		fprintf(stderr, "Incorrect number of arguments!\n");
		return NUM_OF_ARGUMENT_ERROR;
	}

	bool args_flags[MAX_NUM_OF_ARGS - 1];

	for (int i = 1; i < argc; i++) {

		if (sscanf(argv[i], "--gsz=%ld", grid_size)) {
			if (args_flags[0] == true) return error_msg("Grid size argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[0] = true;
		} else if (sscanf(argv[i], "--eps=%lf", eps)) {
			if (args_flags[2] == true) return error_msg("Epsilon argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[2] = true;
		} else if (sscanf(argv[i], "--min=%lf", rand_min_border)) {
			if (args_flags[3] == true) return error_msg("Min border argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[3] = true;
		} else if (sscanf(argv[i], "--max=%lf", rand_max_border)) {
			if (args_flags[4] == true) return error_msg("Max border argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[4] = true;
		} else return error_msg("Incorrect argument!\n", INCORRECT_ARGUMENT);

	}

	if (*grid_size <= 0) return error_msg("Incorrect grid size value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*eps <= 0) return error_msg("Incorrect epsilon value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*rand_min_border >= *rand_max_border) return error_msg("Min border is greater than max border!\n", INCORRECT_ARGUMENT_VALUE);

	return 0;

}
