#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "functions.h"

extern fun_xy f_functions[];
extern fun_xy g_functions[];
const char *f_names[] = {"default"};
const char *g_names[] = {"default"};

bool cas(double *number, double old, double new_one) {
	if (*number != old)
		return false;
	*number = new_one;
	return true;
}

int pprint(double **u, grid_t *grid, char *algo_name, int index_f, int index_g) {

	printf("Was used the following params:\neps = %lf\ngrid_size = %ld\n", grid->eps, grid->grid_size);

	int rc = 0;
	char file_name[MAX_FILE_NAME_SIZE];
	if (snprintf(file_name, MAX_FILE_NAME_SIZE, "gsz=%ld_eps=%lf_f=%s_g=%s_algo_%s", grid->grid_size, grid->eps,
				 f_names[index_f], g_names[index_g], algo_name) <= 0) {
		return error_msg("Couldn't generate file name!\n", INCORRECT_ARGUMENT_VALUE);
	}
	FILE *output_file = fopen(file_name, "w");

	if (!output_file) return FILE_OPEN_ERROR;

	for (int i = 0; i < grid->grid_size + 2; i++) {

		for (int j = 0; j < grid->grid_size + 2; j++) {
			if (fprintf(output_file, "%.2f ", u[i][j]) <= 0) {
				rc = FILE_WRITE_ERROR;
				break;
			}
		}

		if (fprintf(output_file, "\n") <= 0) rc = FILE_WRITE_ERROR;
		if (rc) break;
	}

	fclose(output_file);
	if (rc) fprintf(stderr, "Cannot write the results to the file\n");
	return rc;
}

double randfrom(double min, double max) {
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

int min(int x, int y) {
	return (x < y) ? x : y;
}

int error_msg(char *msg, int err) {
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

void matrix_init(double **u, grid_t *grid, double min_border, double max_border, fun_xy g) {
	double x, y, h = grid->h;
	srand(SEED);

	for (size_t i = 0; i < grid->grid_size + 2; i++) {
		for (size_t j = 0; j < grid->grid_size + 2; j++) {
			x = i * h;
			y = j * h;
			if (x == 0 || x == 1 || y == 0 || y == 1) u[i][j] = g(x, y);
			else u[i][j] = randfrom(min_border, max_border);
		}
	}

}

int arg_parse(int argc, char **argv, size_t *grid_size, double *eps, double *rand_min_border, double *rand_max_border,
			  int *index_f, int *index_g) {

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
		} else if (sscanf(argv[i], "--f=%d", index_f)) {
			if (args_flags[5] == true) return error_msg("Max border argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[5] = true;
		} else if (sscanf(argv[i], "--g=%d", index_g)) {
			if (args_flags[6] == true) return error_msg("Max border argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[6] = true;
		} else return error_msg("Incorrect argument!\n", INCORRECT_ARGUMENT);

	}

	if (*index_f < 0 || *index_f >= MAX_NUM_OF_F_FUNCS) return error_msg("Incorrect f function index!\n", FUNCTION_ERROR);
	if (*index_g < 0 || *index_g >= MAX_NUM_OF_G_FUNCS) return error_msg("Incorrect g function index!\n", FUNCTION_ERROR);
	if (*grid_size <= 0) return error_msg("Incorrect grid size value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*eps <= 0) return error_msg("Incorrect epsilon value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*rand_min_border >= *rand_max_border)
		return error_msg("Min border is greater than max border!\n", INCORRECT_ARGUMENT_VALUE);

	return 0;

}

int num_threads_parse(int argc, char **argv, int *num_threads) {

	bool is_defined = false;

	for (int i = 1; i < argc; i++) {
		if (sscanf(argv[i], "--threads=%d", num_threads)) {
			if (is_defined == true)
				return error_msg("Number of threads argument was listed twice!\n", LISTED_TWICE_ERROR);
			else is_defined = true;
		}
	}

	if (*num_threads < 0) return error_msg("Number of threads is less than zero!\n", INCORRECT_ARGUMENT_VALUE);

	return 0;
}
