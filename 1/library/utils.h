#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_GSZ 100
#define DEFAULT_THREADS 8
#define DEFAULT_EPS 0.1
#define SEED 0xebac0c
#define DEFAULT_BSZ 32
#define MAX_NUM_OF_ARGS 8
#define DEFAULT_RAND_MIN -100
#define DEFAULT_RAND_MAX 100
#define MAX_NUM_OF_F_FUNCS 1
#define MAX_NUM_OF_G_FUNCS 1
#define MAX_FILE_NAME_SIZE 255


typedef double (*fun_xy)(double, double);

typedef struct grid {
	double eps;
	size_t grid_size;
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
	INCORRECT_ARGUMENT_VALUE = -4,
	FILE_OPEN_ERROR = -5,
	FILE_WRITE_ERROR = -6,
	FUNCTION_ERROR = -7,
	FILE_NAME_ERROR = -8
};

bool cas(double *number, double old, double new_one);
double randfrom(double min, double max);
int min(int x, int y);
int error_msg(char *msg, int err);
void matrix_free(double **u, size_t grid_size);
double **matrix_malloc(size_t size);
void matrix_init(double **u, grid_t *grid, double min_border, double max_border, fun_xy g);
int arg_parse(int argc, char **argv, size_t *grid_size, double *eps, double *rand_min_border, double *rand_max_border,
			  int *index_f, int *index_g);

int pprint(double **u, grid_t *grid, char *file_name, int index_f, int index_g);
int num_threads_parse(int argc, char **argv, int *num_threads);
