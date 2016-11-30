#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define NDIM                    3
#define NUM_PART                3000
#define EPS                     1.0e-9
#define ITER                    100
#define DOMAIN_SIZE             1.0e+22
#define MASS_MAX                1.0e+37 /* kg */
#define G                       6.6726e-11
#define DT                      1.0e4
#define IMGDIM                  1000
#define IMGLIM                  IMGDIM * IMGDIM



void swap(double **a, double **b) {
    double              *tmp = *a;
    *a = *b;
    *b = tmp;
}

void initialize_particles(double *mass, double *xa, double *ya, double *za) {
    int                 i;
    
    srand(time(NULL));
    
    for (i = 0; i < NUM_PART; i++) {
        mass[i] = MASS_MAX * (double)(rand() / ((double)RAND_MAX + 1.0));
        
        xa[i] = DOMAIN_SIZE * (double)(rand() / ((double)RAND_MAX + 1.0));
        ya[i] = DOMAIN_SIZE * (double)(rand() / ((double)RAND_MAX + 1.0));
        za[i] = DOMAIN_SIZE * (double)(rand() / ((double)RAND_MAX + 1.0));
        printf("%f\n", mass[i]);
    }
    
}

void write_data(double *snapx, double *snapy, char *out_buffer, int n) {
    int                 loc, x, y, a;
    char                frame[47];
    
    for (a = 0; a < NUM_PART; a++) {        
        x = (snapx[a] / DOMAIN_SIZE) * IMGDIM;        
        y = (snapy[a] / DOMAIN_SIZE) * IMGDIM;
        
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
        x = (snapx[a] / DOMAIN_SIZE) * IMGDIM;        
        y = (snapy[a] / DOMAIN_SIZE) * IMGDIM;
                
        loc = x + (IMGDIM * y);
        if (loc >= 0 && loc < IMGLIM) {        
            out_buffer[loc] = 0;
        }    
    }
}

int main (int argc, char** argv) {
    double              start, end, start1, end1;

    double              *xa, *ya, *za;
    double              *xb, *yb, *zb;
    
    double              *vx, *vy, *vz;
    
    double              *mass;
    
    char                *out_buffer;
    
    double              dx, dy, dz, ax, ay, az, a, dist;
    
    int                 i, j, frame;
    int                 mem_size;
    
    
    mem_size = NUM_PART * sizeof(double);
    
    xa = (double *) malloc(mem_size);
    ya = (double *) malloc(mem_size);
    za = (double *) malloc(mem_size);
    
    xb = (double *) malloc(mem_size);
    yb = (double *) malloc(mem_size);
    zb = (double *) malloc(mem_size);
    
    mass = (double *) malloc(mem_size);
    
    vx = (double *) malloc(mem_size);
    vy = (double *) malloc(mem_size);
    vz = (double *) malloc(mem_size);
    
    out_buffer = (char *) calloc(IMGDIM * IMGDIM, sizeof(char));
    
    initialize_particles(mass, xa, ya, za);
        
    for (frame = 0; frame < ITER; frame++) {
        
        start = MPI_Wtime();
        for (i = 0; i < NUM_PART; i++) { // for particle i
            
            ax = 0, ay = 0, az = 0;
            for (j = 0; j< NUM_PART; j++) { // calculate force based on all other particles
                dx = xa[j] - xa[i];
                dy = ya[j] - ya[i];
                dz = ya[j] - za[i];
                
                dist = sqrt(dx * dx + dy * dy + dz * dz) + 1;
                
                /*if (dist > DOMAIN_SIZE / 2) {
                    continue;
                }*/
                
                a = (G * mass[j]) / (dist * dist);
                
                ax += a * dx; /* accumulate the acceleration from gravitational attraction */
                ay += a * dy;
                az += a * dz;               
                
                vx[i] += DT * ax; /* update velocity of particle "i" */
                vy[i] += DT * ay;
                vz[i] += DT * az;
            }
        
            xb[i] = xa[i] + DT * vx[i]; /* update position of particle "i" */
            yb[i] = ya[i] + DT * vy[i];
            zb[i] = za[i] + DT * vz[i];

            
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
        
        write_data(xb, yb, out_buffer, frame);
        
        end1 = MPI_Wtime();     
        
        printf("%f\t%f\n", end-start, end1-start1);
        
        
        swap(&xa, &xb);
        swap(&ya, &yb);
        swap(&za, &zb);
    }
    
}

