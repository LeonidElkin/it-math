#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

#include "../library/utils.h"
#include "../library/functions.h"

int grid_calculation(grid_t grid, double **u) {

    double dmax, temp, dm, h = grid.h;

    do {
        dmax = 0;
        for (int i = 1; i < grid.grid_size + 1; i++) {
			for (int j = 1; j < grid.grid_size + 1; j++) {
				temp = u[i][j];
				u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1] - h * h * grid.f(i * h, j * h));
				dm = fabs(temp - u[i][j]);
				if (dmax < dm) dmax = dm;
			}
		}
    } while (dmax > grid.eps);
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
	double eps = DEFAULT_EPS;
	double rand_min_border = DEFAULT_RAND_MIN;
	double rand_max_border = DEFAULT_RAND_MAX;

	int rc = arg_parse(argc, argv, &grid_size, &eps, &rand_min_border, &rand_max_border);
	if (rc) return rc;

	double h = 1.0 / (grid_size + 1);
	double **u = matrix_malloc(grid_size + 2);
	grid_t grid = {eps, grid_size, f, h};
	matrix_init(u, grid, rand_min_border, rand_max_border);
	
	test_results_t res = run_test(grid, u);

	// printf("Count of iterations = %d\nTime = %lf\n", res.iterations, res.time);

	for(int i = 0; i < grid_size + 2; i++) {
		for(int j = 0; j < grid_size + 2; j++) {
			printf("%.2f ", u[i][j]);
		}
		printf("\n");
	}

	matrix_free(u, grid_size + 2);

	return 0;
}