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
#include <unistd.h>
#include <math.h> 
#include "mpi.h"

#define MAX 10
#define FOUR_MB_BUFFER_SIZE 4194304

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
    int         max =               186;
    int         n =                 0;
    int         cont =              1;
    char        hostname[15];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
        
    size_buffer = (double *)calloc(FOUR_MB_BUFFER_SIZE, sizeof(double));

    if (my_rank == 0) {
        printf("MPI timer resolution: %1.20f\n", MPI_Wtick());
    }
    
    gethostname(hostname, 15);
    printf("My rank: %d\t%s\n", my_rank, hostname);

    for (size = 1; size < FOUR_MB_BUFFER_SIZE; size *= 2) {            
        while(cont) {
            if (my_rank == 0) {
                MPI_Barrier(comm);
                start = MPI_Wtime();
                for (pass = 0; pass < max; pass++) {
                    MPI_Send(size_buffer, size, MPI_DOUBLE, 1, 0, comm);
                    MPI_Recv(size_buffer, size, MPI_DOUBLE, 1, 0, comm, &status);
                }
                finish = MPI_Wtime();
                raw_time = (finish - start) / max;
                timing_data[n] = raw_time;
                
                cont = Calc_Confidence_Interval_stop(timing_data, n, size);
                MPI_Send(&cont, 1, MPI_DOUBLE, 1, 0, comm);     
            } else { /* my_rank == 1 */
    	        MPI_Barrier(comm); 
                for (pass = 0; pass < max; pass++) {
                    MPI_Recv(size_buffer, size, MPI_DOUBLE, 0, 0, comm, &status); 
                    MPI_Send(size_buffer, size, MPI_DOUBLE, 0, 0, comm);
    	        }
                MPI_Recv(&cont, 1, MPI_DOUBLE, 0, 0, comm, &status); 
            }
        n++;
        }
        max -= 8;
        cont = 1;
        n = 0;
    }  

    MPI_Finalize();
}  /* main */

int Calc_Confidence_Interval_stop(double timing_data[10], int n, int size) {
    double      sum =               0.0;
    double      mean =              0.0;
    double      std_dev =           0.0;
    double      marg_err =          0.0;
    double      marg_perc =         100.0;
    
    if (n > 1) {
        for (int i = 0; i < n; i++) {
            sum += timing_data[i];
        }
        mean = sum / n;
        sum = 0.0;
        for (int i = 0; i < n; i++) {
            sum += pow(timing_data[i] - mean, 2);
        }
        std_dev = sqrt(sum / n);
        marg_err = 1.96 * (std_dev / sqrt(n));
        marg_perc = (marg_err / mean) * 100;
    } else {
        return 1;
    }
    if (marg_perc > 5.0  && n < 15) {
        return 1;
    } else {
        printf("%d\t%f\t%1.10f\t%1.10f\t%f\t%d\n", size, mean, std_dev, marg_err, marg_perc, n);        
        return 0;
    }
}
