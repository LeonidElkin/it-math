#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 100
#define EPS 0.0001
#define SEED 0xebac0c
#define BLOCK_SIZE 64

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

int min(int x, int y) {
	return (x < y) ? x : y;
}

double block(int block_x, int block_y, double **u) {

	int actual_border_x = min(BLOCK_SIZE * (block_x + 1), N) + 1;
	int actual_border_y = min(BLOCK_SIZE * (block_y + 1), N) + 1;
	double temp, delta, d, dm = 0;
	
	for (int i = 1 + BLOCK_SIZE * block_x; i < actual_border_x; i++) {
		for (int j  = 1 + BLOCK_SIZE * block_y; j < actual_border_y; j++) {
			temp = u[i][j];
			u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]);
			d = fabs(temp - u[i][j]);
			if (dm < d) dm = d;
		}
	}
	
	return dm;

}

void poisson(double eps, double **u) {
	int i, j;
	int num_blocks = (N % BLOCK_SIZE) > 0 ? N / BLOCK_SIZE + 1 : N / BLOCK_SIZE;
	double dmax, temp, d, dmax_temp, dm[num_blocks];

	do {

		dmax = 0;

		for (int nx = 0; nx < num_blocks; nx++) {
			dm[nx] = 0;
			#pragma omp parallel for shared(nx, dm) private(i, j, d)
			for (i = 0; i < nx + 1; i++) {
				j = nx - i;
				d = block(i, j, u);
				if (dm[nx] < d) dm[nx] = d;
			}
		}

		for (int nx = num_blocks - 2; nx > -1; nx--) {
			#pragma omp parallel for shared(u, nx, dm) private(i, j, temp, d)
			for (i = 0; i < nx + 1; i++) {
				j = 2 * (num_blocks - 1) - nx - i; 
				d = block(i, j, u);
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
