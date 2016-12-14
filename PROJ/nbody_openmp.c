#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>


// Include global variables. Only this file needs the #define
#define __MAIN 
#include "globals.h"
#undef __MAIN

// User includes
#include "pprintf.h"
#include "helper.h"


int main (int argc, char** argv) {
    double              start, end, start_writing, end_writing, start_tot, end_tot;    
    double              a[4];
    double              dist[4];
    double              dp[12];
    double              ap[12];
    double              total_frame_time                                                = 0;
    
    int                 i, j, frame;
    
    MPI_Init(&argc, &argv);
    
    globals_init();
    parse_args(argc, argv);
        
    Particles_a = (Particle *) malloc(num_part * sizeof(Particle));
    Particles_b = (Particle *) malloc(num_part * sizeof(Particle));

    if (writing) {
        out_buffer = (char *) calloc(img_dim * img_dim, sizeof(char));
    }
    
    if (num_threads != -1) {
        omp_set_num_threads(num_threads);
    }
    
    initialize_particles();
    
    start_tot = MPI_Wtime();
        
    for (frame = 0; frame < num_iter; frame++) {
        
        start_writing = MPI_Wtime();
           
        /*if (writing) {
            write_data_serial_unroll(frame);
        }*/
        
        end_writing = MPI_Wtime(); 
        
        start = MPI_Wtime();
                 
        #pragma omp parallel for schedule(static) private(dist, a, ap, dp, i, j) shared(frame, Particles_a, Particles_b, dt)
        for (i = 0; i < num_part; i++) { // for particle i
    
            ap[0] = 0, ap[1] = 0, ap[2] = 0;
            ap[3] = 0, ap[4] = 0, ap[5] = 0;
            ap[6] = 0, ap[7] = 0, ap[8] = 0;
            ap[9] = 0, ap[10] = 0, ap[11] = 0;
            for (j = 0; j< num_part; j += 4) { // calculate force based on all other particles
                dp[0] = Particles_a[j].pos[X] - Particles_a[i].pos[X];
                dp[1] = Particles_a[j].pos[Y] - Particles_a[i].pos[Y];
                dp[2] = Particles_a[j].pos[Z] - Particles_a[i].pos[Z];
                
                dp[3] = Particles_a[j + 1].pos[X] - Particles_a[i].pos[X];
                dp[4] = Particles_a[j + 1].pos[Y] - Particles_a[i].pos[Y];
                dp[5] = Particles_a[j + 1].pos[Z] - Particles_a[i].pos[Z];
                
                dp[6] = Particles_a[j + 2].pos[X] - Particles_a[i].pos[X];
                dp[7] = Particles_a[j + 2].pos[Y] - Particles_a[i].pos[Y];
                dp[8] = Particles_a[j + 2].pos[Z] - Particles_a[i].pos[Z];
                
                dp[9] = Particles_a[j + 3].pos[X] - Particles_a[i].pos[X];
                dp[10] = Particles_a[j + 3].pos[Y] - Particles_a[i].pos[Y];
                dp[11] = Particles_a[j + 3].pos[Z] - Particles_a[i].pos[Z];
        
                dist[0] = sqrt(dp[0] * dp[0] + dp[1] * dp[1] + dp[2] * dp[2]) + 1;
                dist[1] = sqrt(dp[3] * dp[3] + dp[4] * dp[4] + dp[5] * dp[5]) + 1;
                dist[2] = sqrt(dp[6] * dp[6] + dp[7] * dp[7] + dp[8] * dp[8]) + 1;
                dist[3] = sqrt(dp[9] * dp[9] + dp[1] * dp[1] + dp[11] * dp[11]) + 1;
        
                a[0] = (G * Particles_a[j].mass) / (dist[0] * dist[0] * dist[0] * EPS);
                a[1] = (G * Particles_a[j + 1].mass) / (dist[1] * dist[1] * dist[1] * EPS);
                a[2] = (G * Particles_a[j + 2].mass) / (dist[2] * dist[2] * dist[2] * EPS);
                a[3] = (G * Particles_a[j + 3].mass) / (dist[3] * dist[3] * dist[3] * EPS);
        
                ap[0] += a[0] * dp[0]; /* accumulate the acceleration from gravitational attraction */
                ap[1] += a[0] * dp[1];
                ap[2] += a[0] * dp[2];
                
                ap[3] += a[1] * dp[3];
                ap[4] += a[1] * dp[4];
                ap[5] += a[1] * dp[5];
                
                ap[6] += a[2] * dp[6];
                ap[7] += a[2] * dp[7];
                ap[8] += a[2] * dp[8];
                
                ap[9] += a[3] * dp[9];
                ap[10] += a[3] * dp[10];
                ap[11] += a[3] * dp[11];
        
                Particles_a[i].vel[X] += dt * ap[0]; /* update velocity of particle "i" */
                Particles_a[i].vel[Y] += dt * ap[1];
                Particles_a[i].vel[Z] += dt * ap[2];
                
                Particles_a[i].vel[X] += dt * ap[3];
                Particles_a[i].vel[Y] += dt * ap[4];
                Particles_a[i].vel[Z] += dt * ap[5];
                
                Particles_a[i].vel[X] += dt * ap[6];
                Particles_a[i].vel[Y] += dt * ap[7];
                Particles_a[i].vel[Z] += dt * ap[8];
                
                Particles_a[i].vel[X] += dt * ap[9];
                Particles_a[i].vel[Y] += dt * ap[10];
                Particles_a[i].vel[Z] += dt * ap[11];
            }
    
            Particles_b[i].pos[X] = Particles_a[i].pos[X] + dt * Particles_a[i].vel[X]; /* update position of particle "i" */
            Particles_b[i].pos[Y] = Particles_a[i].pos[Y] + dt * Particles_a[i].vel[Y];
            Particles_b[i].pos[Z] = Particles_a[i].pos[Z] + dt * Particles_a[i].vel[Z]; 
        }
        
        end = MPI_Wtime();
        
        printf("Iteration %d:\t%.10f seconds\t%.10f seconds\n", frame, end - start, end_writing - start_writing);
        total_frame_time += end - start;
        
        swap(&Particles_b, &Particles_a);
    }
    
    end_tot = MPI_Wtime();
    
    printf("Total computation time was: %f\t\tAverage frame time was: %f\t\tAverage Particle interations per second were: %f\n", end_tot - start_tot, total_frame_time / num_iter, (double) (num_part * num_part) / (total_frame_time / num_iter));
    
    MPI_Finalize();
    
}