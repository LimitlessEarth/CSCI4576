#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>
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
    double              dx, dy, dz, ax, ay, az, a, dist;
    double              total_frame_time                                                = 0;
    
    int                 i, j, frame;  
    
    MPI_Init(&argc, &argv);
    
    globals_init();
    parse_args(argc, argv);
        
    Particles_a = (Particle *) malloc(num_part * sizeof(Particle));
    Particles_b = (Particle *) malloc(num_part * sizeof(Particle));
    
    if (writing) {
        out_buffer = (char *) calloc(img_len, sizeof(char));
    }
    
    initialize_particles();
    
    if (num_threads != -1) {
        omp_set_num_threads(num_threads);
    }
    
    start_tot = MPI_Wtime();
        
    for (frame = 0; frame < num_iter; frame++) {
        
        start_writing = MPI_Wtime();     
        if (writing) {
            write_data_serial(frame);
        }
        
        end_writing = MPI_Wtime(); 
        
        start = MPI_Wtime();
        
        #pragma omp parallel for schedule(static) private(dist, dx, dy, dz, a, ax, ay, az, i, j) shared(frame, Particles_a, Particles_b, dt)
        for (i = 0; i < num_part; i++) { // for particle i
            ax = 0, ay = 0, az = 0;
            for (j = 0; j< num_part; j++) { // calculate force based on all other particles
                dx = Particles_a[j].pos[X] - Particles_a[i].pos[X];
                dy = Particles_a[j].pos[Y] - Particles_a[i].pos[Y];
                dz = Particles_a[j].pos[Z] - Particles_a[i].pos[Z];
                
                dist = sqrt(dx * dx + dy * dy + dz * dz) + 1;
                
                if (dist > DOMAIN_SIZE) {
                    continue;
                }
                
                a = (G * Particles_a[j].mass) / (dist * dist * dist * EPS);
                
                ax += a * dx; /* accumulate the acceleration from gravitational attraction */
                ay += a * dy;
                az += a * dz;
                
                Particles_a[i].vel[X] += dt * ax; /* update velocity of particle "i" */
                Particles_a[i].vel[Y] += dt * ay;
                Particles_a[i].vel[Z] += dt * az;
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

