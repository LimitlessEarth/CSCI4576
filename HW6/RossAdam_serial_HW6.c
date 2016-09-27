/* HW6 Dense Matrix Multiplication Serial 
 * 
 * 
 * Name: Adam Ross
 *
 * Input: none
 * Output: Printed Matricies to show correctness
 *
 * 
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "mpi.h"

#define MAX 10
#define BUFSIZE 20
#define FOUR_MB_BUFFER_SIZE 4194304
#define N 8

void print_matrix(int **matrix);

void print_usage() {
    printf("Usage: -f file containging an nxn dense matrix sperated by spaces.\n");
}

main(int argc, char* argv[]) {
    int                 i, j, k;
    int                 **A;
    int                 **B;
    int                 **C;
    int                 sum;
    

    // Malloc our 2d array
    // A
    A = (int **)malloc(N * sizeof(int*));
    A[0] = malloc(N * N * sizeof(int));
    for (i = 1; i < N; i++) {
        A[i] = A[0] + (i * N);
    }
    
    // B
    B = (int **)malloc(N * sizeof(int*));
    B[0] = malloc(N * N * sizeof(int));
    for (i = 1; i < N; i++) {
        B[i] = B[0] + (i * N);
    }
    
    // C
    C = (int **)malloc(N * sizeof(int*));
    C[0] = malloc(N * N * sizeof(int));
    for (i = 1; i < N; i++) {
        C[i] = C[0] + (i * N);
    }
    
    // initalize to 0-N^2
    // A
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = i*N + j;
        }
    }
    // B
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            B[i][j] = i*N + j;
        }
    }
    
    // C
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            C[i][j] = 0;
        }
    }
    
    // Print the initial arrays
    printf("Start. Matrix A\n");
    print_matrix(A);
    printf("\nStart. Matrix B\n");
    print_matrix(B);
    
    // Multiply Matricies
    for (i = 0; i < N; i++) {
       for (j = 0; j < N; j++) {
          sum = 0;
          for (k = 0; k < N; k++) {
             sum = sum + A[i][k] * B[k][j];
          }
          C[i][j] = sum;
       }
    }
 

    
    // Print the end trans formed result
    printf("\nResult Matrix C\n");
    print_matrix(C);
    
}  /* main */

/* Helped method to print the whol matrix */
void print_matrix(int **matrix) {
    int i;
    int j;
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}