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
#include <math.h>
#include "mpi.h"

#define MAX 25
#define N 9

double **Allocate_Square_Matrix(int size);
void Generate_Matrix_Values(double **matrix, int type, int size);
void print_matrix(double **matrix);
int Calc_Confidence_Interval_stop(double timing_data[MAX], int n);

void print_usage() {
    printf("Usage: -f file containging an nxn dense matrix sperated by spaces.\n");
}

main(int argc, char* argv[]) {
    int                 i, j, k;
    double              **A;
    double              **B;
    double              **C;
    double              sum;
    double              start, finish;
    double              timing_data[10];
    int                 cont = 1;
    int                 n;
    
    
    MPI_Init(&argc, &argv);

    // Malloc our 2d array
    A = Allocate_Square_Matrix(N);
    B = Allocate_Square_Matrix(N);
    C = Allocate_Square_Matrix(N);
    
    // initalize to 0-N^2
    Generate_Matrix_Values(A, N, N);
    Generate_Matrix_Values(B, N, N);
    Generate_Matrix_Values(C, 0, N);
    
    // Print the initial arrays
    printf("Start. Matrix A\n");
    print_matrix(A);
    printf("Start. Matrix B\n");
    print_matrix(B);
    
    
    while(cont) {        
        start = MPI_Wtime();
        
        // Multiply Matricies
        for (i = 0; i < N; i++) {
           for (j = 0; j < N; j++) {
              sum = 0.0;
              for (k = 0; k < N; k++) {
                 sum = sum + A[i][k] * B[k][j];
              }
              C[i][j] = sum;
           }
        }
 
        finish = MPI_Wtime();
        timing_data[n] = (finish - start);
        
        cont = Calc_Confidence_Interval_stop(timing_data, n);
        
        n++;
    }

    
    // Print the end trans formed result
    printf("Result Matrix C\n");
    print_matrix(C);
    
    MPI_Finalize();
    
}  /* main */

/*
 * Helper function to allocate 2D array of ints
 * Input: Order of the array
 */
double **Allocate_Square_Matrix(int size) {
    double              **matrix;
    int                 i;
    
    matrix = (double **) malloc(size * sizeof(double*));
    matrix[0] = malloc(size * size * sizeof(double));
    
    for (i = 1; i < size; i++) {
        matrix[i] = matrix[0] + (i * size);
    }
    
    return matrix;
}

/*
 * Helper method to generate matricies with either all 0s or numbers 0-N^2
 * Input: the matrix to set, a type either 0 or non 0, the matrix order
 */
void Generate_Matrix_Values(double **matrix, int type, int size) {
    int                 i, j;
        
    if (type > 0) {
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                matrix[i][j] = (double) (i * size + j);
            }
        }
    } else {
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                matrix[i][j] = 0.0;
            }
        } 
    }
}

/* 
 * Helper method to print a square matrix
 * Input: a matrix and the order of that matrix
 */
void print_matrix(double **matrix) {
    int i;
    int j;
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%.0f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/* Helper function calculate the confidence interval, error margins and determine 
 * if we should keep looping. 
 * Returns 1 or 0 for conintue or stop.
*/
int Calc_Confidence_Interval_stop(double timing_data[MAX], int n) {
    double      sum =               0.0;
    double      mean =              0.0;
    double      std_dev =           0.0;
    double      marg_err =          0.0;
    double      marg_perc =         100.0;
    int         i;
    
    if (n > 5) {
        for (i = 0; i < n; i++) {
            sum += timing_data[i];
        }
        mean = sum / (double) n;
        sum = 0.0;
        for (i = 0; i < n; i++) {
            sum += pow(timing_data[i] - mean, 2);
        }
        std_dev = sqrt(sum / n);
        marg_err = 1.96 * (std_dev / sqrt(n));
        marg_perc = (marg_err / mean) * 100;
    } else {
        return 1;
    }
    if (marg_perc > 5.0  && n < MAX) {
        return 1;
    } else {
        printf("%1.20f\t%1.10f\t%1.10f\t%f\t%d\n", mean, std_dev, marg_err, marg_perc, n); 
        return 0;
    }
}