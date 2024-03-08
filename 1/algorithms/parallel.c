#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

#include "../library/utils.h"
#include "../library/functions.h"

extern fun_xy f_functions[];
extern fun_xy g_functions[];

int grid_calculation(grid_t *grid, double **u) {

    double dmax, temp, dm, h = grid->h, d, dmax_temp;
    int i, j, iter = 0;

    do {
        dmax = 0;
        iter++;
        #pragma omp parallel for shared(u, dmax) private(i, j, temp, d, dm, dmax_temp)
        for (i = 1; i < grid->grid_size + 1; i++) {
            dm = 0;
			for (j = 1; j < grid->grid_size + 1; j++) {
				temp = u[i][j];
				u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1] - h * h * grid->f(i * h, j * h));
				d = fabs(temp - u[i][j]);
				if (dm < d) dm = d;
			}
            while (true) {
				dmax_temp = dmax;
				if (dmax_temp < dm) {
					if (cas(&dmax, dmax_temp, dm))
						break;
				} else
					break;
			}
		}
    } while (dmax > grid->eps);
}

test_results_t run_test(grid_t *grid, double **u) {

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
    fun_xy f = f_functions[0];
	fun_xy g = g_functions[0];

	int rc = arg_parse(argc, argv, &grid_size, &eps, &rand_min_border, &rand_max_border, &f, &g);
	if (rc) return rc;

	double h = 1.0 / (grid_size + 1);
	double **u = matrix_malloc(grid_size + 2);

	grid_t grid = {eps, grid_size, f, h};
	grid_t *grid_p = &grid;
	matrix_init(u, grid_p, rand_min_border, rand_max_border);
	
	test_results_t res = run_test(grid_p, u);

	printf("Count of iterations = %d\tTime = %lf\n", res.iterations, res.time);

	if (rc = pprint(u, grid_p, "parallel_results.txt")) return rc;

	matrix_free(u, grid_size + 2);

	return 0;
}