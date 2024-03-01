#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_GSZ 100
#define DEFAULT_EPS 0.1
#define SEED 0xebac0c
#define DEFAULT_BSZ 32
#define MAX_NUM_OF_ARGS 4

typedef double (*fun_xy)(double, double);

typedef struct grid {
	double eps;
	size_t grid_size;
	size_t block_size;
	fun_xy f;
	double h;
} grid_t;

typedef struct test_results {
    int iterations;
    double time;
} test_results_t;

enum {
	NUM_OF_ARGUMENT_ERROR = -1,
	INCORRECT_ARGUMENT = -2,
	LISTED_TWICE_ERROR = -3,
	INCORRECT_ARGUMENT_VALUE = -4
};

bool cas(double *number, double old, double new_one);
double randfrom(double min, double max);
int min(int x, int y);
int error_msg(char* msg, int err);
void matrix_free(double **u, size_t grid_size);
double **matrix_malloc(size_t size);
