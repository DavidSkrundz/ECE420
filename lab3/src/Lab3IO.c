/*
 Modified `Lab3IO.c` from Development Kit Lab3 (from eClass)
 */

#include "Lab3IO.h"

#include <stdio.h>
#include <stdlib.h>

int Lab3LoadInput(double*** A, int* size) {
	FILE* ip;
	int i,j;
	if ((ip = fopen("data_input","r")) == NULL) {
		printf("error opening the input data.\n");
		return 1;
	}
	fscanf(ip, "%d\n\n", size);
	(*A) = CreateMat(*size, (*size) + 1);
	for (i = 0; i < *size; ++i) {
		for(j = 0; j < *size; ++j)
			fscanf(ip, "%lf\t", &(*A)[i][j]);
		fscanf(ip, "\n");
	}
	fscanf(ip, "\n");
	for (i = 0; i < *size; ++i) {
		fscanf(ip, "%lf\n", &(*A)[i][(*size - 1) + 1]);
	}
	fclose(ip);
	return 0;
}

int Lab3SaveOutput(double* x, int size, double Time) {
	FILE* op;
	int i;
	if ((op = fopen("data_output","w")) == NULL) {
		printf("Error opening the output file.\n");
		return 1;
	}
	fprintf(op, "%d\n", size);
	for (i = 0; i < size; ++i) {
		fprintf(op, "%e\t", x[i]);
	}
	fprintf(op, "\n%lf", Time);
	fclose(op);
	return 0;
}

double** CreateMat(int NumRow, int NumCol) {
	int i;
	double ** A;
	A = malloc(NumRow * sizeof(double*));
	for (i = 0; i < NumRow; ++i) {
		A[i] = malloc(NumCol * sizeof(double));
	}
	return A;
}

int DestroyMat(double** A, int NumRow) {
	int i;
	for (i = 0; i < NumRow; ++i) {
		free(A[i]);
	}
	free(A);
	return 0;
}

int PrintMat(double** A, int NumRow, int NumCol) {
	int i, j;
	for (i = 0; i < NumRow; ++i) {
		for (j = 0; j < NumCol; ++j) {
			printf("%f\t", A[i][j]);
		}
		printf("\n");
	}
	return 0;
}

double* CreateVec(int size) {
	double *b;
	b =  malloc(size * sizeof(double));
	return b;
}

int PrintVec(double* b, int size) {
	int i;
	for (i = 0; i< size; ++i) {
		printf("%f\n", b[i]);
	}
	return 0;
}

int DestroyVec(double* b) {
	free(b);
	return 0;
}
