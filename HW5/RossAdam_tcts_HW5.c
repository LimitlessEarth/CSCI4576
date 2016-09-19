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
#include "mpi.h"

#define MAX 2
#define FOUR_MB_BUFFER_SIZE 4194304

main(int argc, char* argv[]) {
    int         p;
    int         my_rank;
    double      *size_buffer;
    int         size;
    int         pass;
    MPI_Status  status;
    double      start, finish;
    double      raw_time;
    MPI_Comm    comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
    
    size_buffer = (double *)calloc(FOUR_MB_BUFFER_SIZE, sizeof(double));

    if (my_rank == 0) {
        for (size = 1; size < FOUR_MB_BUFFER_SIZE; size *= 2) {
            MPI_Barrier(comm);
            start = MPI_Wtime();
            for (pass = 0; pass < MAX; pass++) {
                MPI_Send(size_buffer, size, MPI_DOUBLE, 1, 0, comm);
                MPI_Recv(size_buffer, size, MPI_DOUBLE, 1, 0, comm, &status);
            }
            finish = MPI_Wtime();
            raw_time = (finish - start) / MAX;
            printf("%d %f\n", size, raw_time);
        }
    } else { /* my_rank == 1 */
        for (size = 1; size < FOUR_MB_BUFFER_SIZE; size *= 2) {
	        MPI_Barrier(comm); 
            for (pass = 0; pass < MAX; pass++) {
                MPI_Recv(size_buffer, size, MPI_DOUBLE, 0, 0, comm, &status); 
                MPI_Send(size_buffer, size, MPI_DOUBLE, 0, 0, comm);
	        } 
        } 
    }


    MPI_Finalize();
}  /* main */
