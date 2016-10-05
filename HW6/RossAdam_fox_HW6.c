/* HW6 Dense Matrix Multiplication parallel Fox
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
#define N 9

int                 p;

void print_usage() {
    printf("Usage: -f file containging an nxn dense matrix sperated by spaces.\n");
}

typedef struct GridInfo {
    MPI_Comm            grid_comm;              // communicator for entire grid
    MPI_Comm            row_comm;               // communicator for current row
    MPI_Comm            col_comm;               // communicator for current column
    int                 grid_order;             // the order of grid
    int                 current_row;            // row of current process
    int                 current_col;            // column of current process
    int                 grid_comm_rank;         // rank of current process in grid_comm
} GridInfo;

double **Allocate_Square_Matrix(int size);
void Generate_Matrix_Values(double **matrix, int type, int size);
void Local_Matrix_Product(double **A, double **B, double **C, int local_n);
int Setup_Grid(GridInfo *grid);
double **Get_Sub_Matrix(double **matrix, GridInfo *grid);
void Aggregate_Matrix(double **sub_matrix, double **matrix, GridInfo *grid);
int Fox(GridInfo *grid, double **local_A, double **local_B, double **local_C);
void print_matrix(double **matrix, int size);
int Calc_Confidence_Interval_stop(double timing_data[MAX], int n, int my_rank);


main(int argc, char* argv[]) {    
    int                 i, j, k;
    double              **A;
    double              **B;
    double              **C;
    double              **local_A;
    double              **local_B;
    double              **local_C;
    double              local_n;
    GridInfo            grid;
    
    // Malloc our 2d array
    A = Allocate_Square_Matrix(N);
    B = Allocate_Square_Matrix(N);
    C = Allocate_Square_Matrix(N);
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (((int) N % (int) sqrt(p)) != 0) {
        return 1;
    }
    
    Setup_Grid(&grid);
        
    // initalize to 0-N^2
    Generate_Matrix_Values(A, N, N);
    Generate_Matrix_Values(B, N, N);
    Generate_Matrix_Values(C, 0, N);

    local_n = N / sqrt(p);
    
    local_A = Allocate_Square_Matrix(local_n);
    local_B = Allocate_Square_Matrix(local_n);
    local_C = Allocate_Square_Matrix(local_n);

    
    if (grid.grid_comm_rank == 0) {
        // Print the initial arrays
        printf("Start. Matrix A\n");
        print_matrix(A, N);
        printf("Start. Matrix B\n");
        print_matrix(B, N);
    }
        
    local_A = Get_Sub_Matrix(A, &grid);
    local_B = Get_Sub_Matrix(B, &grid);
    
    
    if (grid.grid_comm_rank == 0) {
        // Print the initial arrays
        printf("Local A\n");
        print_matrix(local_A, local_n);
        printf("Local B\n");
        print_matrix(local_B, local_n);
    }
  
    Fox(&grid, local_A, local_B, local_C);
 
	// after calculation the processes are synchronized again

	//MPI_Barrier(MPI_COMM_WORLD);

	Aggregate_Matrix(local_C, C, &grid);
    
    if (grid.grid_comm_rank == 0) {
        // Print the end result
        printf("\nResult Matrix C\n");
        print_matrix(C, N);
    } 
    
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
 * Helper method to multiply submatricies
 * Input: The Matricies to be multiplied A and B, C to be stored in and their order local_n
 */
void Local_Matrix_Product(double **A, double **B, double **C, int local_n) {
    int                 i, j, k;
    for (i = 0; i < local_n; i++) {
        for (j = 0; j < local_n; j++) {
            C[i][j] = 0.0;
            for (k = 0; k < local_n; k++) {
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }    
}

/* 
 * Setup our cartesian communicators 
 * Input: a pointer to our grid struct
 */
int Setup_Grid(GridInfo *grid) {
    int                 flag;
    int                 local_ns[2], periods[2], grid_coords[2], sub_coords[2];
    
    grid->grid_order = (int) sqrt((double)p);

    local_ns[0] = local_ns[1] = grid->grid_order;
    periods[0] = periods[1] = 1;
    
    // create communicator for the process grid
    MPI_Cart_create(MPI_COMM_WORLD, 2, local_ns, periods, 1, &(grid->grid_comm));

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
 * This function assigns to each process the correct submatrix
 * Input: The larger matrix and grid info
 */
double **Get_Sub_Matrix(double **matrix, GridInfo *grid) {
    int                 x, y;
    int                 local_n, p_rank, coords[2];    
    double              **sub_matrix;
     
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
 * Aggregate_Matrix is called by each process to return the calculated local_C submatrix.
 * Input: this process's submatrix, the desired result matrix, and grid info
 */
void Aggregate_Matrix(double **sub_matrix, double **matrix, GridInfo *grid) {
    int                 x,y;
    int                 local_n, p_rank, coords[2], i, size;    
    MPI_Status          status;
    
    local_n = N / grid->grid_order;
    
    MPI_Comm_rank(grid->grid_comm, &p_rank);
    MPI_Cart_coords(grid->grid_comm, p_rank, 2, coords);    

    if (p_rank) {
        MPI_Send (&sub_matrix[0][0], local_n * local_n, MPI_DOUBLE, 0, 0, grid->grid_comm); 
        MPI_Send (coords, 2, MPI_INT, 0, 0, grid->grid_comm); }
    else {
        //printf ("My submatrix is \n");
        //print_matrix(sub_matrix, local_n);

        for (x = 0; x < local_n; x++) {
            for (y = 0; y < local_n; y++) {
                 matrix[local_n * coords[0] + x][local_n * coords[1] + y] = sub_matrix[x][y];
            }
        } 

        MPI_Comm_size(grid->grid_comm, &size);
        for (i = 1; i < size; i++) {

            MPI_Recv(&sub_matrix[0][0], local_n * local_n, MPI_DOUBLE, i, 0, grid->grid_comm, &status); 
            MPI_Recv(coords, 2, MPI_INT, i, 0, grid->grid_comm, &status); 
        
            //print_matrix(sub_matrix, local_n);

            for (x = 0; x < local_n; x++) {
                for ( y = 0; y < local_n; y++) {
                     matrix[local_n * coords[0] + x][local_n * coords[1] + y] = sub_matrix[x][y];
                }
            }
        }
    }
}    

/* 
 * Fox algorithm as follow the slides provided by Michael
 * Input: the grib struct and this process's local square matricies A, B, C
 */
int Fox(GridInfo *grid, double **local_A, double **local_B, double **local_C) {
    int                 i;
    int                 source, dest;    
    int                 root;
    int                 local_n = N / grid->grid_order;
    double              **temp_matrix; 
    MPI_Status          status;
    double              start, finish;
    double              timing_data[MAX];
    int                 cont = 0;
    int                 cont_recv = 0;
    int                 n = 0;

    // these are the source and destination ranks 
    // for circular shift of B elements
    dest = (grid->current_row + grid->grid_order - 1) % grid->grid_order;
    source = (grid->current_row + 1) % grid->grid_order; 

    //if (grid->grid_order == 0) printf("hello\n");
    temp_matrix = Allocate_Square_Matrix(local_n);

    Generate_Matrix_Values(local_C, 0, local_n);
    Generate_Matrix_Values(temp_matrix, 0, local_n);
    
    /*if (grid->grid_comm_rank == 2) {
        printf("My Fox matrix is\n");
        print_matrix(temp_matrix, local_n);
        print_matrix(local_A, local_n);
        print_matrix(local_B, local_n);
        print_matrix(local_C, local_n);
    }*/
    
    //printf ("Grid order is: %d\n", grid->grid_order);
    while(cont_recv == 0){
        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();

        for (i = 0; i < grid->grid_order; i++) {
            //printf("Current Stage: %d\n", i);
            root = (grid->current_row + i) % grid->grid_order;
            //printf("Root process is %d, Grid Column is %d, Current Process is %d\n", root, grid->current_col, grid->grid_comm_rank);

            if (root == grid->current_col) {
                MPI_Bcast(*local_A, local_n * local_n, MPI_DOUBLE, root, grid->row_comm);
                Local_Matrix_Product(local_A, local_B, local_C, local_n);
            } else {
                MPI_Bcast(*temp_matrix, local_n * local_n, MPI_DOUBLE, root, grid->row_comm);
                Local_Matrix_Product(temp_matrix, local_B, local_C, local_n);
             }
             // The circular shift and replacement of local_B values
             MPI_Sendrecv_replace(*local_B, local_n * local_n, MPI_DOUBLE, dest, 0, source, 0, grid->col_comm, &status);
         }
     
         finish = MPI_Wtime();
         timing_data[n] = (finish - start);         
     
         cont = Calc_Confidence_Interval_stop(timing_data, n, grid->grid_comm_rank);         
         
         MPI_Allreduce(&cont, &cont_recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
         //if (grid->grid_comm_rank == 2) {
             //printf("hello %d \n", n);
             //}
         
         n++;
    }
    return 1;
}

/* 
 * Helper method to print a square matrix
 * Input: a matrix and the order of that matrix
 */
void print_matrix(double **matrix, int size) {
    int i, j;
    
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
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