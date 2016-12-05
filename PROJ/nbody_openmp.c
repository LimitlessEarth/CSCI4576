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
    double              dx, dy, dz, ax, ay, az, a, dist;
    
    int                 i, j, frame;  
    
    globals_init();
    parse_args(argc, argv);
        
    Particles_a = (Particle *) malloc(num_part * sizeof(Particle));
    Particles_b = (Particle *) malloc(num_part * sizeof(Particle));
    
    if (writing) {
        out_buffer = (char *) calloc(img_len, sizeof(char));
    }
    
    initialize_particles();
    
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
                
                a = (G * Particles_a[j].mass) / (dist * dist * EPS);
                
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
        
        printf("%f\t%f\n", end-start, end_writing-start_writing);
        
        swap(&Particles_b, &Particles_a);
    }
    
    end_tot = MPI_Wtime();
    printf("Total computation time was: %f", end_tot - start_tot);
    
}

