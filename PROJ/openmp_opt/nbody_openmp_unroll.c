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

void swap_dubs(double **a, double **b);
void write_data_serial_unroll(int n, double *xa, double *ya);

int main (int argc, char** argv) {
    double              start, end, start_writing, end_writing, start_tot, end_tot;   
    double              dx0, dy0, dz0, dx1, dy1, dz1, dx2, dy2, dz2, dx3, dy3, dz3;
    double              ax0, ay0, az0, ax1, ay1, az1, ax2, ay2, az2, ax3, ay3, az3;
    double              dist0, dist1, dist2, dist3;
    double              a0, a1, a2, a3;
    double              *xa, *ya, *za, *xb, *yb, *zb;
    double              *xva, *yva, *zva; 
    double              *mass;
    double              total_frame_time                                                = 0;
    
    int                 i, j, frame;    
                        
    MPI_Init(&argc, &argv);
        
    globals_init();
    parse_args(argc, argv);    
    
    xa = (double *) calloc(num_part, sizeof(double));
    ya = (double *) calloc(num_part, sizeof(double));
    za = (double *) calloc(num_part, sizeof(double));
    
    xva = (double *) calloc(num_part, sizeof(double));
    yva = (double *) calloc(num_part, sizeof(double));
    zva = (double *) calloc(num_part, sizeof(double));
    
    xb = (double *) malloc(num_part * sizeof(double));
    yb = (double *) malloc(num_part * sizeof(double));
    zb = (double *) malloc(num_part * sizeof(double));
    
    mass = (double *) malloc(num_part * sizeof(double));
        
    if (writing) {
        out_buffer = (char *) calloc(img_dim * img_dim, sizeof(char));
    }
    
    if (num_threads != -1) {
        omp_set_num_threads(num_threads);
    }
    
    for (i = 0; i < num_part; i++) {
        mass[i] = MASS_MAX * (double)(rand() / ((double)RAND_MAX + 1.0));

        xa[i] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
        ya[i] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
        za[i] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
    }
        
    start_tot = MPI_Wtime();
            
    for (frame = 0; frame < num_iter; frame++) {
        
        start_writing = MPI_Wtime();
           
        if (writing) {
            write_data_serial_unroll(frame, xa, ya);
        }
                        
        end_writing = MPI_Wtime(); 
                
        start = MPI_Wtime();
    
        #pragma omp parallel for schedule(static) private(dx0, dy0, dz0, dx1, dy1, dz1, dx2, dy2, dz2, dx3, dy3, dz3, ax0, ay0, az0, ax1, ay1, az1, ax2, ay2, az2, ax3, ay3, az3, dist0, dist1, dist2, dist3, a0, a1, a2, a3, i, j) shared(frame, xa, ya, za, xb, yb, zb, xva, yva, zva, dt)
        for (i = 0; i < num_part; i++) { // for particle i
                
            ax0 = 0, ay0 = 0, az0 = 0;
            ax1 = 0, ay1 = 0, az1 = 0;
            ax2 = 0, ay2 = 0, az2 = 0;
            ax3 = 0, ay3 = 0, az3 = 0;
            
            for (j = 0; j < num_part; j += 4) { // calculate force based on all other particles
                dx0 = xa[j] - xa[i];
                dy0 = ya[j] - ya[i];
                dz0 = za[j] - za[i];
                
                dx1 = xa[j + 1] - xa[i];
                dy1 = ya[j + 1] - ya[i];
                dz1 = za[j + 1] - za[i];
                
                dx2 = xa[j + 2] - xa[i];
                dy2 = ya[j + 2] - ya[i];
                dz2 = za[j + 2] - za[i];
                
                dx3 = xa[j + 3] - xa[i];
                dy3 = ya[j + 3] - ya[i];
                dz3 = za[j + 3] - za[i];
                
        
                dist0 = sqrt(dx0 * dx0 + dy0 * dy0 + dz0 * dz0) + 1;
                dist1 = sqrt(dx1 * dx1 + dy1 * dy1 + dz1 * dz1) + 1;
                dist2 = sqrt(dx2 * dx2 + dy2 * dy2 + dz2 * dz2) + 1;
                dist3 = sqrt(dx3 * dx3 + dy3 * dy3 + dz3 * dz3) + 1;
                
        
                /*if (dist[0] > DOMAIN_SIZE) {
                    continue;
                }
                if (dist[1] > DOMAIN_SIZE) {
                    continue;
                }
                if (dist[2] > DOMAIN_SIZE) {
                    continue;
                }
                if (dist[3] > DOMAIN_SIZE) {
                    continue;
                }*/
                        
                a0 = (G * mass[j]) / (dist0 * dist0 * dist0 * EPS);
                a1 = (G * mass[j + 1]) / (dist1 * dist1 * dist1 * EPS);
                a2 = (G * mass[j + 2]) / (dist2 * dist2 * dist2 * EPS);
                a3 = (G * mass[j + 3]) / (dist3 * dist3 * dist3 * EPS);
                
        
                ax0 += a0 * dx0; /* accumulate the acceleration from gravitational attraction */
                ay0 += a0 * dy0;
                az0 += a0 * dz0;
                
                ax1 += a1 * dx1; /* accumulate the acceleration from gravitational attraction */
                ay1 += a1 * dy1;
                az1 += a1 * dz1;
                
                ax2 += a2 * dx2; /* accumulate the acceleration from gravitational attraction */
                ay2 += a2 * dy2;
                az2 += a2 * dz2;
                
                ax3 += a3 * dx3; /* accumulate the acceleration from gravitational attraction */
                ay3 += a3 * dy3;
                az3 += a3 * dz3;
                
        
                xva[i] += dt * ax0; /* update velocity of particle "i" */
                yva[i] += dt * ay0;
                zva[i] += dt * az0;
                
                xva[i] += dt * ax1; /* update velocity of particle "i" */
                yva[i] += dt * ay1;
                zva[i] += dt * az1;
                
                xva[i] += dt * ax2; /* update velocity of particle "i" */
                yva[i] += dt * ay2;
                zva[i] += dt * az2;
                
                xva[i] += dt * ax3; /* update velocity of particle "i" */
                yva[i] += dt * ay3;
                zva[i] += dt * az3;
                
            }
    
            xb[i] = xa[i] + dt * xva[i]; /* update position of particle "i" */
            yb[i] = ya[i] + dt * yva[i];
            zb[i] = za[i] + dt * zva[i];  
        }
        
        end = MPI_Wtime();
        
        printf("Iteration %d:\t%.10f seconds\t%.10f seconds\n", frame, end - start, end_writing - start_writing);
        total_frame_time += end - start;
        
        swap_dubs(&xa, &xb);
        swap_dubs(&ya, &yb);
        swap_dubs(&za, &zb);
    }
    
    end_tot = MPI_Wtime();
    
    printf("Total computation time was: %f\t\tAverage frame time was: %f\t\tAverage Particle interations per second were: %f\n", end_tot - start_tot, total_frame_time / num_iter, (double) (num_part * num_part) / (total_frame_time / num_iter));
    
    MPI_Finalize();
    
}

void swap_dubs(double **a, double **b) {
    double              *tmp = *a;
    *a = *b;
    *b = tmp;
}

void write_data_serial_unroll(int n, double *xa, double *ya) {
    int                 loc, x, y, a;
    char                frame[47];
    
    for (a = 0; a < num_part; a++) {        
        x = (xa[a] / DOMAIN_SIZE) * img_dim;        
        y = (ya[a] / DOMAIN_SIZE) * img_dim;
        
        if (x < 0 || x > img_dim)
            continue;
        
        loc = x + (img_dim * y);
        if (loc >= 0 && loc < img_len) {        
            out_buffer[loc] = 255;
        }
        
    }
           
    sprintf(frame, "img/%d.pgm", n);
    FILE *file = fopen(frame, "w");
    fprintf(file, "P5\n");
    fprintf(file, "%d %d\n", img_dim, img_dim);
    fprintf(file, "%d\n", 255);
    fwrite(out_buffer, sizeof(char), img_len, file);
    fclose(file);
        
    for (a = 0; a < num_part; a++) {
        x = (xa[a] / DOMAIN_SIZE) * img_dim;        
        y = (ya[a] / DOMAIN_SIZE) * img_dim;
                
        if (x < 0 || x > img_dim)
            continue;
                
        loc = x + (img_dim * y);
        if (loc >= 0 && loc < img_len) {        
            out_buffer[loc] = 0;
        }    
    }    
}