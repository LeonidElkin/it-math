#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"

double f(double x, double y) {
	return 0.0;
}

double g(double x, double y) {
	if (y == 0) return 100 - 200 * x;
	if (x == 0) return 100 - 200 * y;
	if (y == 1) return 200 * x - 100;
	if (x == 1) return 200 * y - 100;
}

void matrix_init(double **u, size_t grid_size, double h, fun_xy g) {
	double x, y;
	srand(SEED);

	for (int i = 0; i < grid_size + 2; i++) {
		for (int j = 0; j < grid_size + 2; j++) {
			x = i * h; y = j * h;
			if (x == 0 || x == 1 || y == 0 || y == 1) u[i][j] = g(x, y);
			else u[i][j] = randfrom(-100, 100);
		}
	}

}

double block_calculation(int block_x, int block_y, grid_t grid, double **u) {

	int left_border_x = 1 + grid.block_size * block_x;
	int left_border_y = 1 + grid.block_size * block_y;
	int right_border_x = min(grid.block_size * (block_x + 1), grid.grid_size) + 1;
	int right_border_y = min(grid.block_size * (block_y + 1), grid.grid_size) + 1;
	double temp, delta, d, dm = 0, h = grid.h;

	for (int i = left_border_x; i < right_border_x; i++) {
		for (int j = left_border_y; j < right_border_y; j++) {
			temp = u[i][j];
			u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1] - h * h * grid.f(i * h, j * h));
			d = fabs(temp - u[i][j]);
			if (dm < d) dm = d;
		}
	}

	return dm;

}

int grid_calculation(grid_t grid, double **u) {

	int i, j, iter_cnt = 0;
	int num_blocks = (grid.grid_size % grid.block_size) > 0 ? 
		grid.grid_size / grid.block_size + 1 : grid.grid_size / grid.block_size;
	double dmax, temp, d, dmax_temp, dm[num_blocks];

	do {
		iter_cnt++;
		dmax = 0;

		for (int nx = 0; nx < num_blocks; nx++) {
			dm[nx] = 0;
			#pragma omp parallel for shared(nx, dm) private(i, j, d)
			for (i = 0; i < nx + 1; i++) {
				j = nx - i;
				d = block_calculation(i, j, grid, u);
				if (dm[nx] < d) dm[nx] = d;
			}
		}

		for (int nx = num_blocks - 2; nx > -1; nx--) {
			#pragma omp parallel for shared(nx, dm) private(i, j, d)
			for (i = num_blocks - nx - 1; i < num_blocks; i++) {
				j = 2 * (num_blocks - 1) - nx - i;
				d = block_calculation(i, j, grid, u);
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

		

	} while (dmax > grid.eps);

	return iter_cnt;
}

int arg_parse(int argc, char **argv, size_t *grid_size, size_t *block_size, double *eps) {

	if (argc > MAX_NUM_OF_ARGS) {
		fprintf(stderr, "Incorrect number of arguments!\n");
		return NUM_OF_ARGUMENT_ERROR;
	}

	bool args_flags[MAX_NUM_OF_ARGS - 1];

	for (int i = 1; i < argc; i++) {

		if (sscanf(argv[i], "--gsz=%ld", grid_size)) {
			if (args_flags[0] == true) return error_msg("Grid size argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[0] = true;
		} else if (sscanf(argv[i], "--bsz=%ld", block_size)) {
			if (args_flags[1] == true) return error_msg("Block size argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[1] = true;
		} else if (sscanf(argv[i], "--eps=%lf", eps)) {
			if (args_flags[2] == true) return error_msg("Epsilon argument was listed twice!\n", LISTED_TWICE_ERROR);
			else args_flags[2] = true;
		} else return error_msg("Incorrect argument!\n", INCORRECT_ARGUMENT);
	}

	if (*grid_size <= 0) return error_msg("Incorrect grid size value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*block_size <= 0) return error_msg("Incorrect block size value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*block_size % 32 != 0 ) return error_msg("Block size must be divisible by 32!\n", INCORRECT_ARGUMENT_VALUE);
	if (*eps <= 0) return error_msg("Incorrect epsilon value!", INCORRECT_ARGUMENT_VALUE);

	return 0;

}

test_results_t run_test(grid_t grid, double **u) {

    double t1, t2, dt;
	int iter_cnt;

    t1 = omp_get_wtime();
	iter_cnt = grid_calculation(grid, u);
    t2 = omp_get_wtime();
    dt = t2 - t1;

    return (test_results_t){iter_cnt, dt};
	
}

int main(int argc, char **argv) {

	size_t grid_size = DEFAULT_GSZ;
	size_t block_size = DEFAULT_BSZ;
	double eps = DEFAULT_EPS;

	int rc = arg_parse(argc, argv, &grid_size, &block_size, &eps);
	if (rc) return rc;

	double h = 1.0 / (grid_size + 1);
	double **u = matrix_malloc(grid_size + 2);
	matrix_init(u, grid_size, h, g);
	grid_t grid = {eps, grid_size, block_size, f, h};
	
	test_results_t res = run_test(grid, u);

	printf("Count of iterations = %d\nTime = %lf\n", res.iterations, res.time);

	matrix_free(u, grid_size + 2);

	return 0;
}
