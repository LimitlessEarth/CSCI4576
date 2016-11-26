#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define NDIM                    3
#define NUM_PART                30000
#define EPS                     1.0e-9
#define ITER                    100
#define DOMAIN_SIZE             1.0e+22
#define MASS_MAX                1.0e+37 /* kg */
#define G                       6.6726e-11
#define DT                      1.0e4
#define IMGDIM                  1000
#define IMGLIM                  IMGDIM * IMGDIM

typedef struct Particle {
    double pos[NDIM];
    double vel[NDIM];
    double mass;
    
} Particle;

void swap(Particle **a, Particle **b) {
    Particle              *tmp = *a;
    *a = *b;
    *b = tmp;
}

Particle* initialize_particles(Particle *Particles) {
    int                 i;
    
    srand(time(NULL));
    
    for (i = 0; i < NUM_PART; i++) {
        Particles[i].mass = MASS_MAX * (double)(rand() / ((double)RAND_MAX + 1.0));
        
        Particles[i].pos[0] = DOMAIN_SIZE * (double)(rand() / ((double)RAND_MAX + 1.0));
        Particles[i].pos[1] = DOMAIN_SIZE * (double)(rand() / ((double)RAND_MAX + 1.0));
        Particles[i].pos[2] = DOMAIN_SIZE * (double)(rand() / ((double)RAND_MAX + 1.0));
    }
    
}

void write_data(Particle *cur_part_pos, char *out_buffer, int n) {
    int                 loc, x, y, a;
    char                frame[47];
    
    for (a = 0; a < NUM_PART; a++) {        
        x = (cur_part_pos[a].pos[0] / DOMAIN_SIZE) * IMGDIM;        
        y = (cur_part_pos[a].pos[1] / DOMAIN_SIZE) * IMGDIM;
        
        loc = x + (IMGDIM * y);
        if (loc >= 0 && loc < IMGLIM) {        
            out_buffer[loc] = 255;
        }
        
    }
       
    sprintf(frame, "img/%d.pgm", n);
    FILE *file = fopen(frame, "w");
    fprintf(file, "P5\n");
    fprintf(file, "%d %d\n", IMGDIM, IMGDIM);
    fprintf(file, "%d\n", 255);
    fwrite(out_buffer, sizeof(char), IMGDIM * IMGDIM, file);
    fclose(file);
    
    for (a = 0; a < NUM_PART; a++) {
        x = (cur_part_pos[a].pos[0] / DOMAIN_SIZE) * IMGDIM;        
        y = (cur_part_pos[a].pos[1] / DOMAIN_SIZE) * IMGDIM;
                
        loc = x + (IMGDIM * y);
        if (loc >= 0 && loc < IMGLIM) {        
            out_buffer[loc] = 0;
        }    
    }
}

int main (int argc, char** argv) {
    double              start, end, start1, end1;

    Particle            *Particles_a, *Particles_b;
        
    char                *out_buffer;
    
    double              dx, dy, dz, ax, ay, az, a, dist;
    
    int                 i, j, frame;    
        
    Particles_a = (Particle *) malloc(NUM_PART * sizeof(Particle));
    Particles_b = (Particle *) malloc(NUM_PART * sizeof(Particle));
    
    out_buffer = (char *) calloc(IMGDIM * IMGDIM, sizeof(char));
    
    initialize_particles(Particles_a);
        
    for (frame = 0; frame < ITER; frame++) {
        
        start = MPI_Wtime();
        for (i = 0; i < NUM_PART; i++) { // for particle i
            
            ax = 0, ay = 0, az = 0;
            for (j = 0; j< NUM_PART; j++) { // calculate force based on all other particles
                dx = Particles_a[j].pos[0] - Particles_a[i].pos[0];
                dy = Particles_a[j].pos[1] - Particles_a[i].pos[1];
                dz = Particles_a[j].pos[2] - Particles_a[i].pos[2];
                
                dist = sqrt(dx * dx + dy * dy + dz * dz) + 1;
                
                /*if (dist > DOMAIN_SIZE / 2) {
                    continue;
                }*/
                
                a = (G * Particles_a[j].mass) / (dist * dist);
                
                ax += a * dx; /* accumulate the acceleration from gravitational attraction */
                ay += a * dy;
                az += a * dz;               
                
                Particles_a[i].vel[0] += DT * ax; /* update velocity of particle "i" */
                Particles_a[i].vel[1] += DT * ay;
                Particles_a[i].vel[2] += DT * az;
            }
        
            Particles_b[i].pos[0] = Particles_a[i].pos[0] + DT * Particles_a[i].vel[0]; /* update position of particle "i" */
            Particles_b[i].pos[1] = Particles_a[i].pos[1] + DT * Particles_a[i].vel[1];
            Particles_b[i].pos[2] = Particles_a[i].pos[2] + DT * Particles_a[i].vel[2];

            
            //printf("Particle\t%d\tX: %1.30f\tY: %1.30f\tZ: %1.30f\n", i, (0.5 * DT * DT * ax), (0.5 * DT * DT * ay), (0.5 * DT * DT * az));
            //printf("\n");
            
        }
        
        /*for (int k = 0; k < NUM_PART; k++) {
            printf("Particle %d\tX: %f\tY: %f\tZ: %f\n", k, xb[k], yb[k], zb[k]);
        }
        printf("\n");*/
        //printf("%d\n", frame);
        
        end = MPI_Wtime();
        start1 = MPI_Wtime();     
        
        write_data(Particles_b, out_buffer, frame);
        
        end1 = MPI_Wtime();     
        
        printf("%f\t%f\n", end-start, end1-start1);
        
        
        swap(&Particles_b, &Particles_a);
    }
    
}

