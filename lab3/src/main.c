#include "Lab3IO.h"
#include "timer.h"

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	
	int size;
	double** Au;
	double* X;
	int* index;
	double start, end;
	
	if (argc <= 1) {
		printf("Insufficient input!\n");
		return 1;
	}
	int thread_count = strtol(argv[1], NULL, 10);
	
	/*Calculate the solution by serial code*/
	Lab3LoadInput(&Au, &size);
	X = CreateVec(size);
	index = malloc(size * sizeof(int));
	for (int i = 0; i < size; ++i) {
		index[i] = i;
	}
	
	GET_TIME(start);
	if (size == 1) {
		X[0] = Au[0][1] / Au[0][0];
	} else {
#		pragma omp parallel num_threads(thread_count)
		{
			/*Gaussian elimination*/
			for (int k = 0; k < size - 1; ++k) {
#				pragma omp single
				{
					/*Pivoting*/
					double temp = 0;
					int j = 0;
					for (int i = k; i < size; ++i) {
						if (temp < Au[index[i]][k] * Au[index[i]][k]) {
							temp = Au[index[i]][k] * Au[index[i]][k];
							j = i;
						}
					}
					if (j != k) /*swap*/ {
						int i = index[j];
						index[j] = index[k];
						index[k] = i;
					}
				}
				/*calculating*/
#				pragma omp for schedule(static)
				for (int i = k + 1; i < size; ++i) {
					double temp = Au[index[i]][k] / Au[index[k]][k];
					for (int j = k; j < size + 1; ++j) {
						Au[index[i]][j] -= Au[index[k]][j] * temp;
					}
				}
			}
			
#			pragma omp barrier
			
			/*Jordan elimination*/
			for (int k = size - 1; k > 0; --k) {
#				pragma omp for schedule(static)
				for (int i = k - 1; i >= 0; --i) {
					double temp = Au[index[i]][k] / Au[index[k]][k];
					Au[index[i]][k] -= temp * Au[index[k]][k];
					Au[index[i]][size] -= temp * Au[index[k]][size];
				}
			}
		}
		/*solution*/
		for (int k=0; k < size; ++k) {
			X[k] = Au[index[k]][size] / Au[index[k]][k];
		}
	}
	GET_TIME(end);
	Lab3SaveOutput(X, size, end - start);
	printf("%f\n", end - start);
	
	DestroyVec(X);
	DestroyMat(Au, size);
	free(index);
	return 0;
}
