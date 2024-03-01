#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_GSZ 100
#define DEFAULT_EPS 0.1
#define SEED 0xebac0c
#define DEFAULT_BSZ 32
#define MAX_NUM_OF_ARGS 4

enum {
	NUM_OF_ARGUMENT_ERROR = -1,
	INCORRECT_ARGUMENT = -2,
	LISTED_TWICE_ERROR = -3,
	INCORRECT_ARGUMENT_VALUE = -4
};

bool cas(double *number, double old, double new) {
	if (*number != old)
		return false;
	*number = new;
	return true;
}

double randfrom(double min, double max) {
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

void matrix_init(double **u, size_t grid_size) {
	double x, y;

	for (int i = 0; i < grid_size + 2; i++) {
		for (int j = 0; j < grid_size + 2; j++) {
			x = (double) i / (grid_size + 1);
			y = (double) j / (grid_size + 1);
			if (x == 0 || x == 1)
				u[i][j] = pow(-1, x) * 100 - pow(-1, x) * 200 * y;
			else if (y == 0 || y == 1)
				u[i][j] = pow(-1, y) * 100 - pow(-1, y) * 200 * x;
			else
				u[i][j] = randfrom(-100, 100);
		}
	}
}

int min(int x, int y) {
	return (x < y) ? x : y;
}

double block(int block_x, int block_y, double **u, size_t block_size, size_t grid_size) {

	int actual_border_x = min(block_size * (block_x + 1), grid_size) + 1;
	int actual_border_y = min(block_size * (block_y + 1), grid_size) + 1;
	double temp, delta, d, dm = 0;

	for (int i = 1 + block_size * block_x; i < actual_border_x; i++) {
		for (int j = 1 + block_size * block_y; j < actual_border_y; j++) {
			temp = u[i][j];
			u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]);
			d = fabs(temp - u[i][j]);
			if (dm < d) dm = d;
		}
	}

	return dm;

}

void poisson(double eps, double **u, size_t grid_size, size_t block_size) {
	int i, j;
	int num_blocks = (grid_size % block_size) > 0 ? grid_size / block_size + 1 : grid_size / block_size;
	double dmax, temp, d, dmax_temp, dm[num_blocks];

	do {

		dmax = 0;

		for (int nx = 0; nx < num_blocks; nx++) {
			dm[nx] = 0;
			#pragma omp parallel for shared(nx, dm) private(i, j, d)
			for (i = 0; i < nx + 1; i++) {
				j = nx - i;
				d = block(i, j, u, grid_size, block_size);
				if (dm[nx] < d) dm[nx] = d;
			}
		}

		for (int nx = num_blocks - 2; nx > -1; nx--) {
			#pragma omp parallel for shared(nx, dm) private(i, j, d)
			for (i = num_blocks - nx - 1; i < num_blocks; i++) {
				j = 2 * (num_blocks - 1) - nx - i;
				d = block(i, j, u, grid_size, block_size);
				if (dm[nx] < d) dm[nx] = d;
			}
		}

		#pragma omp parrallel for shared(dm, dmax) private(i, dmax_temp)
		for (i = 1; i < num_blocks; i++) {
			while (true) {
				dmax_temp = dmax;
				if (dmax_temp < dm[i]) {
					if (cas(&dmax, dmax_temp, dm[i]))
						break;
				} else
					break;
			}
		}

	} while (dmax > eps);

}

double **matrix_malloc(size_t grid_size) {
	double **u = (double **) malloc(sizeof(double *) * (grid_size + 2));
	for (int i = 0; i < grid_size + 2; i++)
		u[i] = (double *) malloc(sizeof(double) * (grid_size + 2));
	return u;
}

void matrix_free(double **u, size_t grid_size) {
	for (int i = 0; i < grid_size + 2; i++)
		free(u[i]);
	free(u);
}

int error_msg(char* msg, int err) {
	fprintf(stderr, "%s", msg);
	return err;
}

int arg_parse(int argc, char **argv, size_t *grid_size, size_t *block_size, double *eps) {

	if (argc > MAX_NUM_OF_ARGS) {
		fprintf(stderr, "Incorrect number of arguments!\n");
		return NUM_OF_ARGUMENT_ERROR;
	}

	bool args_flags[MAX_NUM_OF_ARGS - 1];

	for (int i = 1; i < argc; i++) {

		if (fscanf(argv[i], "--gsz=%d", grid_size)) {
			if (args_flags[0] == true) return error_msg("Grid size argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[0] = true;
		} else if (fscanf(argv[i], "--bsz=%d", block_size)) {
			if (args_flags[1] == true) return error_msg("Block size argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[1] = true;
		} else if (fscanf(argv[i], "--eps=%f", eps)) {
			if (args_flags[2] == true) return error_msg("Epsilon argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[2] = true;
		} else return error_msg("Incorrect argument!\n", INCORRECT_ARGUMENT);
	}

	if (*grid_size <= 0) return error_msg("Incorrect grid size value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*block_size <= 0) return error_msg("Incorrect block size value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*block_size % 32 != 0 ) return error_msg("Block size must be divisible by 32!\n", INCORRECT_ARGUMENT_VALUE);
	if (*eps <= 0) return error_msg("Incorrect epsilon value!", INCORRECT_ARGUMENT_VALUE);

}

int main(int argc, char **argv) {

	size_t grid_size = DEFAULT_GSZ, block_size = DEFAULT_BSZ;
	double eps = DEFAULT_EPS;

	arg_parse(argc, argv, &grid_size, &block_size, &eps);

	double **u = matrix_malloc(grid_size);

	srand(SEED);
	matrix_init(u, grid_size);
	poisson(eps, u, grid_size, block_size);

	for (int i = 0; i < grid_size + 2; i++) {
		for (int j = 0; j < grid_size + 2; j++) {
			printf("%.2f ", u[i][j]);
		}
		printf("\n");
	}

	matrix_free(u, grid_size);
}
