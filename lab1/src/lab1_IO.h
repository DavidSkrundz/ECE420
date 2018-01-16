/*
 Modified `lab1_IO.h` from Development Kit Lab1 (from eClass)
 Header for the Lab 1 IO functions
 */

#pragma once

/**
 Allocate memory and load the input data for Lab 1
 
 -----
 Input:
 int*** A    pointer to the matrix A
 int*** B    pointer to the matrix B
 int* n      pointer to the matrix size
 
 original files should be the output of the matrixgen.c with name "data_input"
 in the same folder
 
 -----
 Output:
 Generated matrices will be passed back to the array *A and *B
 
 -----
 Example:
 Two integer array pointers and a integer should be defined before calling this
 function:
 int** A; int** B; int n
 call this function as
 Lab1_loadinput(&A, &B, &n);
 */
int Lab1_loadinput(int*** A, int*** B, int* size);

/**
 Save the data to the file for Lab 1
 
 -----
 Input:
 int** C     pointer to the result matrix
 int* n      pointer to the matrix size
 double Time measure calulation time
 
 -----
 Output:
 data_output the stored data
 
 -----
 Example:
 Lab1_saveoutput(C, &n, Time);
 */
int Lab1_saveoutput(int** C, int* size, double Time);
