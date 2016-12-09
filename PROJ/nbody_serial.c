#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

// Include global variables. Only this file needs the #define
#define __MAIN 
#include "globals.h"
#undef __MAIN

// User includes
#include "pprintf.h"
#include "helper.h"

int main (int argc, char** argv) {
    double              start, end, start_writing, end_writing, start_tot, end_tot;    
    double              ap[0], ap[1], ap[2], a, dist;
    double              *xa, *ya, *za, *xb, *yb, *zb;
    double              *xva, *yva, *zva, *xvb, *yvb, *zvb; 
    double              *dp;
    double              *ap;
    double              total_frame_time                                                = 0;
    
    int                 i, j, frame;
    
    MPI_Init(&argc, &argv);
    
    globals_init();
    parse_args(argc, argv);
        
    dp = (double) calloc (3, sizeof(double));
    ap = (double) calloc (3, sizeof(double));
    
    xa = (double *) calloc(num_part, sizeof(double));
    ya = (double *) calloc(num_part, sizeof(double));
    za = (double *) calloc(num_part * sizeof(double));
    
    xva = (double *) calloc(num_part, sizeof(double));
    yva = (double *) calloc(num_part, sizeof(double));
    zva = (double *) calloc(num_part, sizeof(double));
    
    xb = (double *) malloc(num_part * sizeof(double));
    yb = (double *) malloc(num_part * sizeof(double));
    zb = (double *) malloc(num_part, sizeof(double));
    
    xvb = (double *) malloc(num_part * sizeof(double));
    yvb = (double *) malloc(num_part * sizeof(double));
    zvb = (double *) malloc(num_part * sizeof(double));
    
    mass = (double *) malloc(num_part * sizeof(double));
    
    if (writing) {
        out_buffer = (char *) calloc(img_dim * img_dim, sizeof(char));
    }
    
    for (i = 0; i < num; i++) {
        mass[i] = MASS_MAX * (double)(rand() / ((double)RAND_MAX + 1.0));

        xa[i] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
        ya[i] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
        za[i] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
    }
    
    start_tot = MPI_Wtime();
        
    for (frame = 0; frame < num_iter; frame++) {
        
        start_writing = MPI_Wtime();
           
        if (writing) {
            write_data_serial(frame);
        }
        
        end_writing = MPI_Wtime(); 
        
        start = MPI_Wtime();
        for (i = 0; i < num_part; i++) { // for particle i
    
            ap[0] = 0, ap[1] = 0, ap[2] = 0;
            for (j = 0; j< num_part; j++) { // calculate force based on all other particles
                dp[0] = xa[j] - xa[i];
                dp[1] = ya[j] - ya[i];
                dp[2] = za[j] - za[i];
        
                dist = sqrt(dp[0] * dp[0] + dp[1] * dp[1] + dp[2] * dp[2]) + 1;
        
                if (dist > DOMAIN_SIZE) {
                    continue;
                }
        
                a = (G * mass[j]) / (dist * dist * dist * EPS);
        
                ap[0] += a * dp[0]; /* accumulate the acceleration from gravitational attraction */
                ap[1] += a * dp[1];
                ap[2] += a * dp[2];
        
                xva[i] += dt * ap[0]; /* update velocity of particle "i" */
                yva[i] += dt * ap[1];
                zva[i] += dt * ap[2];
            }
    
            xb[i] = xa[i] + dt * xva[i]; /* update position of particle "i" */
            yb[i] = ya[i] + dt * yva[i];
            zb[i] = za[i] + dt * zva[i];  
        }
        
        end = MPI_Wtime();
        
        printf("Iteration %d:\t%.10f seconds\t%.10f seconds\n", frame, end - start, end_writing - start_writing);
        total_frame_time += end - start;
        
        swap(&xa, &xb);
        swap(&ya, &yb);
        swap(&za, &zb);
        
        swap(&xva, &xvb);
        swap(&yva, &yvb);
        swap(&zva, &zvb);
    }
    
    end_tot = MPI_Wtime();
    
    printf("Total computation time was: %f\t\tAverage frame time was: %f\t\tAverage Particle interations per second were: %f\n", end_tot - start_tot, total_frame_time / num_iter, (double) (num_part * num_part) / (total_frame_time / num_iter));
    
    MPI_Finalize();
    
}

