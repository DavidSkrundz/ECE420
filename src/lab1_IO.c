/*
 Modified `lab1_IO.c` from Development Kit Lab1 (from eClass)
 */

#include <stdio.h>
#include <stdlib.h>
#include "lab1_IO.h"

int Lab1_loadinput(int*** A, int*** B, int* n) {
	FILE* ip;
	
	if ((ip = fopen("data_input","r")) == NULL) {
		printf("error opening the input data.\n");
		return 1;
	}
	fscanf(ip, "%d\n", n);
	
	*A = malloc(*n * sizeof(int*));
	*B = malloc(*n * sizeof(int*));
	
	for (int i = 0; i < *n; i++) {
		(*A)[i] = malloc(*n * sizeof(int));
		(*B)[i] = malloc(*n * sizeof(int));
	}
	
	for (int i = 0; i < *n; i++) {
		for (int j = 0; j< *n; j++) {
			fscanf(ip, "%d\t", &(*A)[i][j]);
		}
	}
	for (int i = 0; i < *n; i++) {
		for (int j = 0; j <* n; j++) {
			fscanf(ip, "%d\t", &(*B)[i][j]);
		}
	}
	fclose(ip);
	return 0;
}

int Lab1_saveoutput(int** C, int* n, double Time) {
	FILE* op;
	
	if ((op = fopen("data_output", "w")) == NULL) {
		printf("Error opening the output file.\n");
		return 1;
	}
	
	fprintf(op, "%d\n\n", *n);
	for (int i = 0; i < *n; i++) {
		for (int j = 0; j < *n; j++) {
			fprintf(op, "%d\t", C[i][j]);
		}
		fprintf(op, "\n");
	}
	fprintf(op, "%f\n", Time);
	fclose(op);
	return 0;
}
