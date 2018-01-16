#include "header.h"
#include "lab1_IO.h"
#include "timer.h"

#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Globals needed by each thread */
int threadCount;
int** A;
int** B;
int** C;
int n;

void* multiply(void* data) {
	int rank = VOIDP2INT(data);
	
	int sqrtP = sqrt(threadCount);
	int x = floor(rank / sqrtP);
	int y = rank % sqrtP;
	
	int minX = x * n / sqrtP;
	int maxX = (x + 1) * n / sqrtP - 1;
	int minY = y * n / sqrtP;
	int maxY = (y + 1) * n / sqrtP - 1;
	
	for (int i = minX; i <= maxX; ++i) {
		for (int j = minY; j <= maxY; ++j) {
			int value = 0;
			for (int r = 0; r < n; ++r) {
				value += A[i][r] * B[j][r];
			}
			C[i][j] = value;
		}
	}
	
	return NULL;
}

void realMain() {
	// Load the input
	Lab1_loadinput(&A, &B, &n);
	
	// Transpose B for better locality
	for (int i = 1; i < n; ++i) {
		for (int j = 0; j < i; ++j) {
			int tmp = B[i][j];
			B[i][j] = B[j][i];
			B[j][i] = tmp;
		}
	}
	
	// Prepare output
	C = malloc(n * sizeof(int*));
	for (int i = 0; i < n; i++) {
		C[i] = malloc(n * sizeof(int));
	}
	
	// Start timer
	double start = 0, end = 0;
	GET_TIME(start);
	
	// Run threads
	pthread_t* threads = malloc(threadCount * sizeof(pthread_t));
	for (int i = 0; i < threadCount; ++i) {
		pthread_create(&threads[i], NULL, multiply, INT2VOIDP(i));
	}
	
	// Wait for threads
	for (int i = 0; i < threadCount; ++i) {
		pthread_join(threads[i], NULL);
	}
	
	// Stop timer
	GET_TIME(end);
	
	// Save the output
	Lab1_saveoutput(C, &n, end - start);
}

void usage(char* name) {
	printf("OVERVIEW: ECE 420 Lab 1\n");
	printf("\n");
	printf("USAGE: %s <thread_count>\n", name);
	printf("\n");
	printf("ARGUMENTS:\n");
	printf("  <thread_count>:      The number of threads to use\n");
	printf("\n");
	printf("OPTIONS:\n");
	printf("  --help, -h           Display available options\n");
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Bad number of arguments.\n");
		usage(argv[0]);
		exit(1);
	}
	if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
		usage(argv[0]);
		exit(0);
	}
	
	char* argv1End = NULL;
	threadCount = (int)strtol(argv[1], &argv1End, 10);
	if (*argv1End != '\0') {
		printf("Non-numeric argument.\n");
		usage(argv[0]);
		exit(1);
	}
	
	realMain();
	
	return 0;
}
