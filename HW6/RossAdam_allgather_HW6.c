/* HW6 Dense Matrix Multiplication parallel
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

#define N 9
#define MAX 25

double **Allocate_Square_Matrix(int size);
void Generate_Matrix_Values(double **matrix, int type, int size);
void Local_Matrix_Product(double **A, double **B, double **C, int local_n);
void print_matrix(double **matrix);
int Calc_Confidence_Interval_stop(double timing_data[MAX], int n, int my_rank);

void print_usage() {
    printf("Usage: -f file containging an nxn dense matrix sperated by spaces.\n");
}

void Generate_Column_Chunks(**Matrix, **Column, int Number_col) {
    return;
}

main(int argc, char* argv[]) {
    int                 p;
    int                 my_rank;
    
    int                 i, j, k;
    
    double              **A; // the whole A
    double              **B; // the whole B
    double              **C; // to store C
    double              **local_A; // row(s) block of a
    double              **vector_B; // Column of B
    double              **local_vector_B; // my chunk of vector B
    double              **local_vector_C; // my end result column/vector chunk of C
    
    MPI_Datatype        column; 
    int                 local_n;
    double              start, finish;
    double              timing_data[MAX];
    int                 cont = 0;
    int                 cont_recv = 0;
    int                 n = 0;

    // Malloc our 2d array
    A = Allocate_Square_Matrix(N);
    B = (double *) malloc(N * sizeof(double*));
    for (i = 0; i < N; i++) {
        B[i] = i;
        //printf("%d\n",B[i]);
    }
    C = Allocate_Square_Matrix(N);
    
    // initalize to 0-N^2
    Generate_Matrix_Values(A, N, N);
    Generate_Matrix_Values(B, N, N);
    Generate_Matrix_Values(C, 0, N);    
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (((int) N % (int) sqrt(p)) != 0) {
        return 1;
    }

    local_n = N / p;
    

    //Generate_Row_Chunk(A, local_A, local_n);
    Generate_Column_Chunk(B, vector_B, N);
    
    // Scatter matrix A to row blocks
    MPI_Scatter(A, N * N, MPI_DOUBLE, local_A, local_n * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, N * N, MPI_DOUBLE, vector_B, local_n * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    /*if (my_rank == 0) {
        // Print the initial arrays
        printf("Start. Matrix A\n");
        print_matrix(A);
        //printf("\nStart. Matrix B\n");
        //print_matrix(B);
    }*/
    
    // Build MPI datatype vector of every Nth item - i.e. a column
    MPI_Type_vector(N, 1, N, MPI_DOUBLE, &column);
    MPI_Type_commit(&column);
    
    /* local_m = m/p, local_n = n/p */
    //double dot_prod;
    
    //int MPI_Allgather(const void *sendbuf, int  sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
    
    while(cont_recv == 0) {        
        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();
        
       /* parallel_mat_vect.c -- computes a parallel matrix-vector product.  Matrix
        *     is distributed by block rows.  Vectors are distributed by blocks. */
        
        // all column-vectors
        for (k = 0; k < N; k++) {
            MPI_Allgather(local_vector_B[0][k], local_n, column, vector_B[k], local_n, MPI_DOUBLE, MPI_COMM_WORLD);
            // for each local vector_peice
            for (i = 0; i < local_row; i++) {
                local_y[i] = 0.0;
                // for each entry in the row
                for (j = 0; j < n; j++) {
                    //calculate the local y : local_vector_peice = local_vector_peice[i] + local_A_ROW_entry[i][j] * global_vector[j]
                    local_y[i] = local_y[i] + local_A[i][j] * global_column[j]
                }
            }
        }
        

        finish = MPI_Wtime();
        timing_data[n] = (finish - start);
    
        cont = Calc_Confidence_Interval_stop(timing_data, n, my_rank);
    
        MPI_Allreduce(&cont, &cont_recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
        n++;
    }
 
    /*if (my_rank == 0) {
        // Print the end trans formed result
        printf("\nResult Matrix C\n");
        print_matrix(C);
    }*/
    
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
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

/* Helper function calculate the confidence interval, error margins and determine 
 * if we should keep looping. 
 * Returns 1 or 0 for conintue or stop.
*/
int Calc_Confidence_Interval_stop(double timing_data[MAX], int n, int my_rank) {
    double      sum =               0.0;
    double      mean =              0.0;
    double      std_dev =           0.0;
    double      marg_err =          0.0;
    double      marg_perc =         100.0;
    int         i;
    
    if (n > 2) {
        for (i = 0; i < n; i++) {
            sum += timing_data[i];
        }
        mean = sum / n;
        sum = 0.0;
        for (i = 0; i < n; i++) {
            sum += pow(timing_data[i] - mean, 2);
        }
        std_dev = sqrt(sum / n);
        marg_err = 1.96 * (std_dev / sqrt(n));
        marg_perc = (marg_err / mean) * 100;
    } else {
        return 0;
    }
    if (marg_perc > 5.0  && n < MAX) {
        return 0;
    } else {
        if (my_rank == 0) {
            printf("%1.20f\t%1.10f\t%1.10f\t%f\t%d\n", mean, std_dev, marg_err, marg_perc, n);
        }        
        return 1;
    }
}