/* HW5 Dense Matrix Transpose
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
#define N 6

void print_matrix(int **matrix);

void print_usage() {
    printf("Usage: -f file containging an nxn dense matrix sperated by spaces.\n");
}

main(int argc, char* argv[]) {
    int                 option = 0;
    char                buf[BUFSIZE + 1];
    int                 p;
    int                 my_rank;
    MPI_Status          status;
    MPI_Comm            comm;
    int                 i;
    int                 j;
    int                 **data;
    
    MPI_Datatype        column;
    double              dense_matrix[6][6] = {
        {1, 2, 3, 4, 5, 6},
        {7, 8, 9, 10, 11, 12},
        {13, 14, 15, 16, 17, 18},
        {19, 20, 21, 22, 23, 24},
        {25, 26, 27, 28, 29, 30},
        {31, 32, 33, 34, 35, 36}
    };

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);

    // Malloc our 2d array
    data = (int **)malloc(N * sizeof(int*));
    data[0] = malloc(N * N * sizeof(int));
    for (i = 1; i < N; i++) {
        data[i] = data[0] + (i * N);
    }
    
    // initalize to 0-N^2
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            data[i][j] = i*N + j;
        }
    }
    
    // Print the initial array store in rank 0
    if(my_rank == 0) {
        printf("Start. My rank: %d", my_rank);
        print_matrix(data);
    }
    
    // Build MPI datatype vector of every Nth item - i.e. a oclumn
    MPI_Type_vector(N, 1, N, MPI_INT, &column);
    MPI_Type_commit(&column);
    
    // Send each column to rank 1
    for (i = 0; i < N; i++) {
        if (my_rank == 0) {
            MPI_Send(&data[0][i], 1, column, 1, 0, comm);
        } else { /* my_rank == 1 */
            MPI_Recv(&data[i][0], N, MPI_INT, 0, 0, comm, &status); 
        }
    }  
    
    // Print the end trans formed result
    if(my_rank == 1) {
        printf("\nEnd. My rank: %d", my_rank);
        print_matrix(data);
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