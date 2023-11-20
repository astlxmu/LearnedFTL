#ifndef UTIL_BBSSD
#define UTIL_BBSSD

#include <stdio.h>
#include <math.h>
#include <time.h>
// #include "lftl.h"
// #include "learnedFTL.h"

// float* regression(uint64_t *, uint64_t *, float, int, float *, float *);

float batch_gradient_descent(uint64_t *, uint64_t *, int, float, float*, float*);

float predict(uint64_t, float*, float *);


void quick_sort(uint64_t lpns[], int low, int high);

// void reclusiveRegression(uint64_t *x, uint64_t *y, int num, lr_node *wbs, int *total);

int least_square(uint64_t *x, uint64_t *y, uint64_t start, int num, float *w, float *b);

int LeastSquareNew(uint64_t *x, uint64_t *y, int num, float *w, float *b);

void LeastSquare(uint64_t *x, uint64_t *y, int num, float *w, float *b);

/**
 * @brief for cmt, create a simple hash table
 * 
 */
uint64_t hash_func(uint64_t key);



#endif