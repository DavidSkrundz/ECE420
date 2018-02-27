#include "Lab3IO.h"
#include "timer.h"

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	int size;
	double** Au;
	double* X;
	double start, end;
	
	if (argc <= 1) { printf("Insufficient input!\n"); exit(1); }
	int thread_count = strtol(argv[1], NULL, 10);
	
	Lab3LoadInput(&Au, &size);
	X = CreateVec(size);
	
	GET_TIME(start);
	if (size > 1) {
#		pragma omp parallel num_threads(thread_count)
		{
			/* Gaussian elimination */
			for (int k = 0; k < size - 1; ++k) {
#				pragma omp single
				{
					/* Pivoting */
					double temp = 0;
					int j = 0;
					for (int i = k; i < size; ++i) {
						if (temp < Au[i][k] * Au[i][k]) {
							temp = Au[i][k] * Au[i][k];
							j = i;
						}
					}
					/* swap */
					if (j != k) {
						double* i = Au[j];
						Au[j] = Au[k];
						Au[k] = i;
					}
				}
				/* calculating */
#				pragma omp for schedule(static)
				for (int i = k + 1; i < size; ++i) {
					double temp = Au[i][k] / Au[k][k];
					for (int j = k; j < size + 1; ++j) {
						Au[i][j] -= Au[k][j] * temp;
					}
				}
			}
			
#			pragma omp barrier
			
			/* Jordan elimination */
			for (int k = size - 1; k > 0; --k) {
#				pragma omp for schedule(static)
				for (int i = k - 1; i >= 0; --i) {
					double temp = Au[i][k] / Au[k][k];
					Au[i][k] -= temp * Au[k][k];
					Au[i][size] -= temp * Au[k][size];
				}
			}
		}
	}
	/* solution */
	for (int k=0; k < size; ++k) {
		X[k] = Au[k][size] / Au[k][k];
	}
	GET_TIME(end);
	
	Lab3SaveOutput(X, size, end - start);
	printf("%f\n", end - start);
	
	DestroyVec(X);
	DestroyMat(Au, size);
	return 0;
}
