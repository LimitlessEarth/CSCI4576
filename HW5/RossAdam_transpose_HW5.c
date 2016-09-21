/* HW5 Alpha-Beta, Dense Matrix Transpose
 * 
 * 
 * Name: Adam Ross
 *
 * Input: none
 * Output: 
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

void build_column_data_type(INDATA_TYPE* indata, MPI_Datatype* message_type_ptr);

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
    
    MPI_Datatype        column;
    double              dense_matrix[6][6] = {
        {1, 2, 3, 4, 5, 6},
        {7, 8, 9, 10, 11, 12},
        {13, 14, 15, 16, 17, 18},
        {19, 20, 21, 22, 23, 24},
        {25, 26, 27, 28, 29, 30},
        {31, 32, 33, 34, 35, 36}
    };
    
    /*
    char                *filename;
    FILE*               file;
    int                 has_file        = 0;

        
    /*while ((option = getopt(argc, argv,"f:")) != -1) {
        switch (option) {
             case 'f' : strncpy(buf, optarg, BUFSIZE);
                 has_file = 1;
                 break;
             default: print_usage(); 
                 exit(1);
        }
    }
    
    if (!has_file) {
        return 1;
    }   
    printf("%s\n", filename);
    file = fopen(filename, "r");*/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
        
    //size_buffer = (double *)calloc(FOUR_MB_BUFFER_SIZE, sizeof(double));
    
    MPI_Type_vector(N, 1, N, MPI_INT, &column);
    MPI_Type_commit(&column);
    
    
    /*
    if (my_rank == 0) {

        }
    } else { /* my_rank == 1 /
 
    }
    */


    MPI_Finalize();
}  /* main */

void print_matrix(int matrix[N][N]) {
    int i;
    int j;
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}