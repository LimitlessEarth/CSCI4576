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
#define N 16

void print_matrix(int **matrix, int size);

void print_usage() {
    printf("Usage: -f file containging an nxn dense matrix sperated by spaces.\n");
}

int                 p;

typedef struct GridInfo {
    int                 p;                      // total number of processed
    MPI_Comm            grid_comm;              // communicator for entire grid
    MPI_Comm            row_comm;               // communicator for current row
    MPI_Comm            col_comm;               // communicator for current column
    int                 grid_order;             // the order of grid
    int                 current_row;            // row of current process
    int                 current_col;            // column of current process
    int                 grid_comm_rank;         // rank of current process in grid_comm
} GridInfo;

/*
 *
 *
 */
int **Allocate_Square_Matrix(int size) {
    int                 **matrix;
    int                 i;
    
    matrix = (int **) malloc(size * sizeof(int*));
    matrix[0] = malloc(size * size * sizeof(int));
    
    for (i = 1; i < size; i++) {
        matrix[i] = matrix[0] + (i * size);
    }
    
    return matrix;
}

/*
 *
 *
 */
void Generate_Matrix_Values(int **matrix, int type) {
    int                 i, j;
    
    if (type > 0) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                matrix[i][j] = i * N + j;
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                matrix[i][j] = 0;
            }
        } 
    }
}

/*
 *
 *
 */
void Local_Matrix_Product(int **A, int **B, int **C, int local_n) {
    int                 i, j, k;
    
    for (i = 0; i < local_n; i++) {
        for (j = 0; j < local_n; j++) {
            C[i][j] = 0.0;
            for (k = 0; k < local_n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

/* 
 * 
 * 
 */
int Setup_Grid(GridInfo *grid) {
    int                 flag, my_rank;
    int                 p;
    // cartesian topology is 2D
    int                 local_ns[2], periods[2], grid_coords[2], sub_coords[2];
    
    // get the process rank and the comm size
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &p);
    
    grid->grid_order = (int) sqrt((double)p);

    local_ns[0] = local_ns[1] = grid->grid_order;
    // a circular shift is required for the second local_nension
    periods[0] = periods[1] = 1;
    
    // create communicator for the process grid
    MPI_Cart_create (MPI_COMM_WORLD, 2, local_ns, periods, 1, &(grid->grid_comm));

    // retrieve the process rank in the grid Communicator 
    // and the process coordinates in the cartesian topology
    MPI_Comm_rank(grid->grid_comm, &(grid->grid_comm_rank));
    MPI_Cart_coords(grid->grid_comm, grid->grid_comm_rank, 2, grid_coords);
    grid->current_row = grid_coords[0];
    grid->current_col = grid_coords[1];

    // setup row communicators
    sub_coords[0] = 0;
    sub_coords[1] = 1;
    MPI_Cart_sub (grid->grid_comm, sub_coords, &(grid->row_comm));

    // setup column communicators
    sub_coords[0] = 1;
    sub_coords[1] = 0;
    MPI_Cart_sub(grid->grid_comm, sub_coords, &(grid->col_comm));
    
    return 0;
}

/* 
 * this function assigns to each process the correct submatrix
 * 
 */
int **Get_Sub_Matrix(int **matrix, GridInfo *grid) {
    int                 x, y;
    int                 local_n, p_rank, coords[2];    
    int                 **sub_matrix;
     
    local_n = N / grid->grid_order;
    sub_matrix = Allocate_Square_Matrix(local_n);
    
    MPI_Comm_rank (grid->grid_comm, &p_rank);
    MPI_Cart_coords (grid->grid_comm, p_rank, 2, coords);
    
    for (x = 0; x < local_n; x++) {
        for (y = 0; y < local_n; y++) {
             sub_matrix[x][y] = matrix[local_n * coords[0] + x][local_n * coords[1] + y]; 
        }
    }
     
    return sub_matrix;
}

/* 
 * 
 * 
 */
// Aggregate_Matrix is called by each process to return the calculated local_C submatrix.
// In this way the final cMatrix of order N is formed  
void Aggregate_Matrix(int **sub_matrix, int **matrix, GridInfo *grid) {
    int                 x,y;
    int                 local_n, p_rank, coords[2], i, size;    
    MPI_Status          status;
    
    local_n = N / grid->grid_order;
    
    MPI_Comm_rank(grid->grid_comm, &p_rank);
    MPI_Cart_coords(grid->grid_comm, p_rank, 2, coords);    

    if (p_rank) {
        MPI_Send (&sub_matrix[0][0], local_n * local_n, MPI_INT, 0, 0, grid->grid_comm); 
        MPI_Send (coords, 2, MPI_INT, 0, 0, grid->grid_comm); }
    else {
        //printf ("My own submatrix is \n");
        print_matrix(sub_matrix, local_n);

        for (x = 0; x < local_n; x++) {
            for (y = 0; y < local_n; y++) {
                 matrix[local_n * coords[0] + x][local_n * coords[1] + y] = sub_matrix[x][y];
            }
        } 

        MPI_Comm_size(grid->grid_comm, &size);
        for (i = 1; i < size; i++) {

            MPI_Recv(&sub_matrix[0][0], local_n * local_n, MPI_INT, i, 0, grid->grid_comm, &status); 
            MPI_Recv(coords, 2, MPI_INT, i, 0, grid->grid_comm, &status); 

            //printf("Process %d received from process %d the coordinates (%d,%d)\n", p_rank, status.MPI_SOURCE, coords[0], coords[1]);
            //printf("Process %d received from process %d the following submatrix\n", p_rank, status.MPI_SOURCE);
        
            print_matrix(sub_matrix, local_n);

            for (x = 0; x < local_n; x++) {
                for ( y = 0; y < local_n; y++) {
                     matrix[local_n * coords[0] + x][local_n * coords[1] + y] = sub_matrix[x][y];
                }
            }
        }
    }
}    

/* 
 * 
 * 
 */
// the implementation of Fox's algorithm
int Fox(GridInfo * grid, int **local_A, int **local_B, int **local_C) {
    int                 i;
    int                 source, dest;    
    int                 x, y;
    int                 root;
    int                 local_n = N / grid->grid_order;
    int                 **temp_matrix; 
    MPI_Status          status;

    // these are the source and destination ranks 
    // for circular shift of B elements
    dest = (grid->current_row + grid->grid_order - 1) % grid->grid_order;
    source = (grid->current_row + 1) % grid->grid_order; 

    temp_matrix = Allocate_Square_Matrix(local_n);
    //temp_matrix = (int *) malloc(sizeof(int) * local_n * local_n);
    Generate_Matrix_Values(local_C, 0);
    print_matrix(temp_matrix, local_n);

    //printf ("Grid order is %d\n", grid->grid_order);

    for (i = 0; i < grid->grid_order; i++) {
         printf("FOX ALGORITHM->STAGE %d\n", i);
         root = (grid->current_row + i) % grid->grid_order;

         printf("Root process is %d, Grid Column is %d, Current Process is %d\n", root, grid->current_col, grid->grid_comm_rank);

         if (root == grid->current_col) {
             // if the current process belongs to the grid main diagocal
             // broadcasts its copy of local_A to the row processes
             /*
             printf("Process %d (diagonal) broadcasting local_A...\n", grid->grid_comm_rank);
             for (x = 0; x < local_n; x++) {
                for (y = 0; y < local_n; y++) {
                    printf("local_A[%d][%d](-%d-)=%d\n", x, y, grid->grid_comm_rank,local_A[x][y]);
                }
             }*/
             MPI_Bcast(local_A, local_n * local_n, MPI_INT, root, grid->row_comm);
             Local_Matrix_Product(local_A, local_B, local_C, local_n);
         } else {
             /*
             // otherewise the temp_matrix is broadcasted
             printf("Process %d broadcasting tempA...\n", grid->grid_comm_rank);
             for (x = 0; x < local_n; x++) {
                 for (y = 0; y < local_n; y++) {
                     printf("tempA[%d][%d](-%d-)=%d\n", x, y, grid->grid_comm_rank, temp_matrix[x][y]);
                 }
             }*/
             printf("hello\n");
             MPI_Bcast(temp_matrix, local_n * local_n, MPI_INT, root, grid->row_comm);
             printf("hello\n");
             Local_Matrix_Product(temp_matrix, local_B, local_C, local_n);
         }
         // the next function performs the circular shift and replacement
         // of local_B values
         MPI_Sendrecv_replace(local_B, local_n * local_n, MPI_INT, dest, 0, source, 0, grid->col_comm, &status);
     }

    return 0;
}

main(int argc, char* argv[]) {
    int                 p;
    int                 my_rank;
    MPI_Status          status;
    MPI_Comm            row_comm;
    MPI_Comm            column_comm;
    
    MPI_Group           group_world;
    
    int                 i, j, k;
    int                 **A;
    int                 **B;
    int                 **C;
    int                 **local_A;
    int                 **local_B;
    int                 **local_C;
    int                 sum;
    MPI_Datatype        column;
    int                 local_n;
    GridInfo            grid;
    
    // Malloc our 2d array
    A = Allocate_Square_Matrix(N);
    B = Allocate_Square_Matrix(N);
    C = Allocate_Square_Matrix(N);
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (((int) N % (int) sqrt(p)) != 0) {
        return 1;
    }
    
    Setup_Grid (&grid);
    
    // initalize to 0-N^2
    Generate_Matrix_Values(A, N);
    Generate_Matrix_Values(B, N);
    Generate_Matrix_Values(C, 0);

    local_n = N / sqrt(p);
    
    local_A = Allocate_Square_Matrix(local_n);
    local_B = Allocate_Square_Matrix(local_n);
    local_C = Allocate_Square_Matrix(local_n);

    if (my_rank == 0) {
        // Print the initial arrays
        printf("Start. Matrix A\n");
        print_matrix(A, N);
        printf("\nStart. Matrix B\n");
        print_matrix(B, N);
    }
        
    local_A = Get_Sub_Matrix(A, &grid);
    local_B = Get_Sub_Matrix(B, &grid);
    
    if (my_rank == 0) {
        // Print the initial arrays
        printf("Local A\n");
        print_matrix(local_A, local_n);
        printf("\nLocal B\n");
        print_matrix(local_B, local_n);
    }
  
    Fox(&grid, local_A, local_B, local_C);
 
	// after calculation the processes are synchronized again

	MPI_Barrier(MPI_COMM_WORLD);

	Aggregate_Matrix(local_C, C, &grid);
 
    if (my_rank == 0) {
        // Print the end trans formed result
        printf("\nResult Matrix C\n");
        print_matrix(C, N);
    }
    
    MPI_Finalize();
}  /* main */

/* Helped method to print the whol matrix */
void print_matrix(int **matrix, int size) {
    int i, j;
    
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}