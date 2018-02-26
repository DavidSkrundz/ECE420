/*
 Modified `Lab3IO.h` from Development Kit Lab3 (from eClass)
 Header for the Lab 3 IO functions
 */

#pragma once

/**
 Allocate memory and load the input data for Lab 3. The returned matrix is the augmented size by (size+1) matrix [A|b]
 
 -----
 Input:
 int ***A    pointer to the augmented matrix [A|b]
 int *size   pointer to the rows of the augmented matrix [A|b]. (Number of columns will be 1 more)
 
 Note: original files should be the output of the datagen.c with name "data_input" in the same folder
 
 -----
 Output:
 Generated matrix will be passed back to the array *A, along with the matrix size in *size
 
 -----
 Example:
 An integer array pointer and a integer should be defined before calling this function:
 int **A; int size;
 call this function as
 Lab3LoadInput(&A, &size);
 */
int Lab3LoadInput(double*** A, int* size);

/**
 Save the data to the file for Lab 3
 
 -----
 Input:
 double* x     pointer to the result vector
 int size      the vector size
 double Time   calulation time
 
 -----
 Output:
 data_output the stored data
 
 -----
 Example:
 Lab3SaveOutput(x, size, Time);
 */
int Lab3SaveOutput(double* x, int size, double time);

/**
 Allocate memory for an array
 -----
 Input:
 NumRow    Number of rows
 NumCol    Number of columns
 -----
 Output:
 ** A    pointer to the new int array
 */
double** CreateMat(int NumRow, int NumCol);

/**
 Free the memory
 -----
 Input:
 NumRow    Number of rows
 */
int DestroyMat(double** A, int NumRow);

/**
 Print an array
 -----
 Input:
 A         pointer to the array
 NumRow    Number of rows
 NumCol    Number of columns
 */
int PrintMat(double** A, int NumRow, int NumCol);

double* CreateVec(int size);
int PrintVec(double* b, int size);
int DestroyVec(double* b);
