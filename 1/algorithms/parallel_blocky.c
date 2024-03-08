#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../library/utils.h"
#include "../library/functions.h"

extern fun_xy f_functions[];
extern fun_xy g_functions[];

double block_calculation(int block_x, int block_y, grid_t *grid, size_t block_size, double **u) {

	int left_border_x = 1 + block_size * block_x;
	int left_border_y = 1 + block_size * block_y;
	int right_border_x = min(block_size * (block_x + 1), grid->grid_size) + 1;
	int right_border_y = min(block_size * (block_y + 1), grid->grid_size) + 1;
	double temp, d, dm = 0, h = grid->h;

	for (int i = left_border_x; i < right_border_x; i++) {
		for (int j = left_border_y; j < right_border_y; j++) {
			temp = u[i][j];
			u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1] - h * h * grid->f(i * h, j * h));
			d = fabs(temp - u[i][j]);
			if (dm < d) dm = d;
		}
	}

	return dm;

}

int grid_calculation(grid_t *grid, size_t block_size, double **u) {

	int i, j, iter_cnt = 0;
	int num_blocks = (grid->grid_size % block_size) > 0 ? 
		grid->grid_size / block_size + 1 : grid->grid_size / block_size;
	double dmax, d, dmax_temp, dm[num_blocks];

	do {

		iter_cnt++;
		dmax = 0;

		for (int nx = 0; nx < num_blocks; nx++) {
			dm[nx] = 0;
			#pragma omp parallel for shared(nx, dm) private(i, j, d)
			for (i = 0; i < nx + 1; i++) {
				j = nx - i;
				d = block_calculation(i, j, grid, block_size, u);
				if (dm[nx] < d) dm[nx] = d;
			}
		}

		for (int nx = num_blocks - 2; nx > -1; nx--) {
			#pragma omp parallel for shared(nx, dm) private(i, j, d)
			for (i = num_blocks - nx - 1; i < num_blocks; i++) {
				j = 2 * (num_blocks - 1) - nx - i;
				d = block_calculation(i, j, grid, block_size, u);
				if (dm[nx] < d) dm[nx] = d;
			}
		}

		#pragma omp parallel for shared(dm, dmax) private(i, dmax_temp)
		for (i = 0; i < num_blocks; i++) {
			while (true) {
				dmax_temp = dmax;
				if (dmax_temp < dm[i]) {
					if (cas(&dmax, dmax_temp, dm[i]))
						break;
				} else
					break;
			}
		}

		

	} while (dmax > grid->eps);

	return iter_cnt;
}

int block_size_parse (int argc, char **argv, size_t *block_size) {

	bool is_defined = false;

	for (int i = 1; i < argc; i++) {
		if (sscanf(argv[i], "--bsz=%ld", block_size)) {
			if (is_defined == true) return error_msg("Block size argument was listed twice!\n", LISTED_TWICE_ERROR);
			else is_defined = true;
		}
	}

	if (*block_size <= 0) return error_msg("Incorrect block size value!\n", INCORRECT_ARGUMENT_VALUE);
	if (*block_size % 32 != 0 ) return error_msg("Block size must be divisible by 32!\n", INCORRECT_ARGUMENT_VALUE);
}

test_results_t run_test(grid_t *grid, size_t block_size, double **u, int num_threads) {

	omp_set_num_threads(num_threads);

    double t1, t2, dt;
	int iter_cnt;

    t1 = omp_get_wtime();
	iter_cnt = grid_calculation(grid, block_size, u);
    t2 = omp_get_wtime();
    dt = t2 - t1;

    return (test_results_t){iter_cnt, dt};
	
}

int main(int argc, char **argv) {

	size_t grid_size = DEFAULT_GSZ;
	size_t block_size = DEFAULT_BSZ;
	double eps = DEFAULT_EPS;
	double rand_min_border = DEFAULT_RAND_MIN;
	double rand_max_border = DEFAULT_RAND_MAX;
	fun_xy f = f_functions[0];
	fun_xy g = g_functions[0];
	int rc, num_threads = DEFAULT_THREADS;

	if (rc = arg_parse(argc, argv, &grid_size, &eps, &rand_min_border, &rand_max_border, &f, &g)) return rc;
	if (rc = block_size_parse(argc, argv, &block_size)) return rc;
	if (rc = num_threads_parse(argc, argv, &num_threads)) return rc;

	double h = 1.0 / (grid_size + 1);
	double **u = matrix_malloc(grid_size + 2);
	grid_t grid = {eps, grid_size, f, h};
	grid_t *grid_p = &grid;
	matrix_init(u, grid_p, rand_min_border, rand_max_border);
	
	test_results_t res = run_test(grid_p, block_size, u, num_threads);

	printf("Count of iterations = %d\tTime = %lf\n", res.iterations, res.time);
	pprint(u, grid_p, "blocky_results.txt");
	printf("block_size = %ld", block_size);

	matrix_free(u, grid_size + 2);

	return 0;
}
