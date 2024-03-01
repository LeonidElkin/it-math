#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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
	for (int i = 0; i < size; i++)
		free(u[i]);
	free(u);
}

double **matrix_malloc(size_t size) {
	double **u = (double **) malloc(sizeof(double *) * (size));
	for (int i = 0; i < size; i++)
		u[i] = (double *) malloc(sizeof(double) * (size));
	return u;
}
