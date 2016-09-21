/* HW5 Alpha-Beta, Dense Matrix Transpose
 * 
 * 
 * Name: Adam Ross
 *
 * Input: none
 * Output: Byte timing data for 1 Byte - 16 KiB
 * on intervals of 5 bytes
 * 
 *
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <math.h> 
#include <stdlib.h>
#include "mpi.h"

#define MAX 10
#define SIXTEEN_KB_BUFFER_SIZE 2048

int Calc_Confidence_Interval_stop(double timing_data[10], int n, int size);

main(int argc, char* argv[]) {
    int         p;
    int         my_rank;
    double      *size_buffer;
    int         size;
    int         pass;
    MPI_Status  status;
    double      start, finish;
    double      raw_time;
    double      timing_data[10];
    MPI_Comm    comm;
    int         max =               128;
    int         n =                 0;
    int         cont =              1;
    char        hostname[30];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
        
    size_buffer = (double *)calloc(SIXTEEN_KB_BUFFER_SIZE, sizeof(double));

    if (my_rank == 0) {
        printf("MPI timer resolution: %1.20f\n", MPI_Wtick());
    }
    
    gethostname(hostname, 15);
    printf("My rank: %d\t%s\n", my_rank, hostname);
    
    // MPI wamup before actual timings
    if (my_rank == 0) {
        MPI_Send(size_buffer, SIXTEEN_KB_BUFFER_SIZE, MPI_DOUBLE, 1, 0, comm);
        MPI_Recv(size_buffer, SIXTEEN_KB_BUFFER_SIZE, MPI_DOUBLE, 1, 0, comm, &status);
    } else {
        MPI_Recv(size_buffer, SIXTEEN_KB_BUFFER_SIZE, MPI_DOUBLE, 0, 0, comm, &status); 
        MPI_Send(size_buffer, SIXTEEN_KB_BUFFER_SIZE, MPI_DOUBLE, 0, 0, comm);
    }

    // Iterate for each desired data size
    for (size = 1; size <= SIXTEEN_KB_BUFFER_SIZE; size+= 5) {            
        while(cont) {
            if (my_rank == 0) {
                // Barrier to kind of sync our timing
                MPI_Barrier(comm);
                // start timing
                start = MPI_Wtime();
                
                // Run the ping pong multiple times to get resolution away from the tick resolution
                for (pass = 0; pass < max; pass++) {
                    MPI_Send(size_buffer, size, MPI_DOUBLE, 1, 0, comm);
                    MPI_Recv(size_buffer, size, MPI_DOUBLE, 1, 0, comm, &status);
                }
                // Finish timing
                finish = MPI_Wtime();
                raw_time = (finish - start) / max;
                
                // Store in array for confidence analysis
                timing_data[n] = raw_time;
                
                // Calculate the confidence error percentage to determine if we need to run more iterations
                cont = Calc_Confidence_Interval_stop(timing_data, n, size);
                MPI_Barrier(comm);
                // Tell process 1
                MPI_Send(&cont, 1, MPI_INT, 1, 0, comm);     
            } else { /* my_rank == 1 */
                // Barrier to kind of sync our timing
    	        MPI_Barrier(comm); 
                
                // Run the ping pong multiple times to get resolution away from the tick resolution
                for (pass = 0; pass < max; pass++) {
                    MPI_Recv(size_buffer, size, MPI_DOUBLE, 0, 0, comm, &status); 
                    MPI_Send(size_buffer, size, MPI_DOUBLE, 0, 0, comm);
    	        }
                MPI_Barrier(comm);
                MPI_Recv(&cont, 1, MPI_INT, 0, 0, comm, &status); 
            }
            n++;
        }
        // Do not need to run as many iterations for larger message sizes
        if (size % 65 == 0) {
            max -= 4;
            if (my_rank == 0) {
                printf("%d\n", max);
            }
        }
        cont = 1;
        n = 0;
    }  

    MPI_Finalize();
}  /* main */


/* Helper function calculate the confidence interval, error margins and determine 
 * if we should keep looping. 
 * Returns 1 or 0 for conintue or stop.
*/
int Calc_Confidence_Interval_stop(double timing_data[10], int n, int size) {
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
        return 1;
    }
    if (marg_perc > 5.0  && n < 20) {
        return 1;
    } else {
        printf("%d\t%1.20f\t%1.10f\t%1.10f\t%f\t%d\n", size, mean, std_dev, marg_err, marg_perc, n);        
        return 0;
    }
}