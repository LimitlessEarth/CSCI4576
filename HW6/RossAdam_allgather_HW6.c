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

#define MAX 10
#define BUFSIZE 20
#define FOUR_MB_BUFFER_SIZE 4194304
#define N 9

void print_matrix(int **matrix);

void print_usage() {
    printf("Usage: -f file containging an nxn dense matrix sperated by spaces.\n");
}

main(int argc, char* argv[]) {
    int                 p;
    int                 my_rank;
    MPI_Status          status;
    MPI_Comm            comm;
    
    int                 i, j, k;
    int                 **A;
    int                 **B;
    int                 **C;
    int                 sum;
    MPI_Datatype        column;
    int                 local_n;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);

    if ((N % sqrt(p)) != 0) {
        return
    }

    local_n = N / sqrt(p);

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
    if (my_rank == 0) {
        // Print the initial arrays
        printf("Start. Matrix A\n");
        print_matrix(A);
        printf("\nStart. Matrix B\n");
        print_matrix(B);
    }
    
    // Build MPI datatype vector of every Nth item - i.e. a oclumn
    MPI_Type_vector(N, 1, N, MPI_INT, &column);
    MPI_Type_commit(&column);
    
    /* local_m = m/p, local_n = n/p */
    double dot_prod;
    
// int MPI_Allgather(const void *sendbuf, int  sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
    
    for (k = 0; k < N; k++) {
        MPI_Allgather(&B[0][k], N, column, &B[0][k], N, column, MPI_COMM_WORLD);
        for (i = 0; i < local_n; i++) {
            C[i][j] = 0.0;
            for (j = 0; j < N; j++) {
                C[i][j] += A[i][j] * B[j][i];
            }
        }
    }
 
    if (my_rank == 0) {
        // Print the end trans formed result
        printf("\nResult Matrix C\n");
        print_matrix(C);
    }
    
    MPI_Finalize();
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