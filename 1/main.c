#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 100
#define EPS 0.0001
#define SEED 0xebac0c

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

void matrix_init(double **u) {
	double x, y;

	for (int i = 0; i < N + 2; i++) {
		for (int j = 0; j < N + 2; j++) {
			x = (double) i / (N + 1);
			y = (double) j / (N + 1);
			if (x == 0 || x == 1)
				u[i][j] = pow(-1, x) * 100 - pow(-1, x) * 200 * y;
			else if (y == 0 || y == 1)
				u[i][j] = pow(-1, y) * 100 - pow(-1, y) * 200 * x;
			else
				u[i][j] = randfrom(-100, 100);
		}
	}
}

void poisson(double eps, double **u) {
	double dmax, temp, d, dmax_temp, dm[N + 1];
	int i, nx, j, cnt = 0;
	
	do {

		dmax = 0;

		for (nx = 1; nx < N + 1; nx++) {
			dm[nx] = 0;
			#pragma omp parallel for shared(u, nx, dm) private(i, j, temp, d)
			for (i = 1; i < nx + 1; i++) {
				j = nx + 1 - i;
				temp = u[i][j];
				u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]);
				d = fabs(temp - u[i][j]);
				if (dm[i] < d) dm[i] = d;
			}
		}

		for (nx = N - 1; nx > 0; nx--) {
			#pragma omp parallel for shared(u, nx, dm) private(i, j, temp, d)
			for (i = N - nx + 1; i < N + 1; i++) {
				j = 2 * N - nx - i + 1;
				temp = u[i][j];
				u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]);
				d = fabs(temp - u[i][j]);
				if (dm[i] < d) dm[i] = d;
			}
		}

		#pragma omp parrallel for shared(dm, dmax) private(i)
		for (i = 1; i < N + 1; i++) {
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

double **matrix_malloc() {
	double **u = (double **) malloc(sizeof(double *) * (N + 2));
	for (int i = 0; i < N + 2; i++)
		u[i] = (double *) malloc(sizeof(double) * (N + 2));
	return u;
}

void matrix_free(double **u) {
	for (int i = 0; i < N + 2; i++)
		free(u[i]);
	free(u);
}

int main() {

	double **u = matrix_malloc();

	srand(SEED);
	matrix_init(u);
	poisson(EPS, u);

	for (int i = 0; i < N + 2; i++) {
		for (int j = 0; j < N + 2; j++) {
			printf("%.2f ", u[i][j]);
		}
		printf("\n");
	}

	matrix_free(u);
}
