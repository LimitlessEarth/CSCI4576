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
    double              *a, *dist;
    double              *xa, *ya, *za, *xb, *yb, *zb;
    double              *xva, *yva, *zva, *xvb, *yvb, *zvb; 
    double              *dp, *ap, *mass;
    double              total_frame_time                                                = 0;
    
    int                 i, j, frame;
    
    MPI_Init(&argc, &argv);
    
    globals_init();
    parse_args(argc, argv);
        
    dp = (double *) calloc (12, sizeof(double));
    ap = (double *) calloc (12, sizeof(double));
    
    dist = (double *) calloc (4, sizeof(double));
    a = (double *) calloc (4, sizeof(double));
    
    xa = (double *) calloc(num_part, sizeof(double));
    ya = (double *) calloc(num_part, sizeof(double));
    za = (double *) calloc(num_part, sizeof(double));
    
    xva = (double *) calloc(num_part, sizeof(double));
    yva = (double *) calloc(num_part, sizeof(double));
    zva = (double *) calloc(num_part, sizeof(double));
    
    xb = (double *) malloc(num_part * sizeof(double));
    yb = (double *) malloc(num_part * sizeof(double));
    zb = (double *) malloc(num_part * sizeof(double));
    
    xvb = (double *) malloc(num_part * sizeof(double));
    yvb = (double *) malloc(num_part * sizeof(double));
    zvb = (double *) malloc(num_part * sizeof(double));
    
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
            write_data_serial_unroll(frame);
        }
        
        end_writing = MPI_Wtime(); 
        
        start = MPI_Wtime();
        
        #pragma omp parallel private(dist, a, ap, dp, i, j) shared(frame, xa, ya, za, xb, yb, zb, xva, yva, zva, dt)
        {
         
        dp = (double *) calloc (12, sizeof(double));
        ap = (double *) calloc (12, sizeof(double));
    
        dist = (double *) calloc (4, sizeof(double));
        a = (double *) calloc (4, sizeof(double));
         
        #pragma omp for schedule(static) 
        for (i = 0; i < num_part; i++) { // for particle i
    
            ap[0] = 0, ap[1] = 0, ap[2] = 0;
            for (j = 0; j < num_part; j += 4) { // calculate force based on all other particles
                dp[0] = xa[j] - xa[i];
                dp[1] = ya[j] - ya[i];
                dp[2] = za[j] - za[i];
                
                dp[3] = xa[j + 1] - xa[i];
                dp[4] = ya[j + 1] - ya[i];
                dp[5] = za[j + 1] - za[i];
                
                dp[6] = xa[j + 2] - xa[i];
                dp[7] = ya[j + 2] - ya[i];
                dp[8] = za[j + 2] - za[i];
                
                dp[9] = xa[j + 3] - xa[i];
                dp[10] = ya[j + 3] - ya[i];
                dp[11] = za[j + 3] - za[i];
        
                dist[0] = sqrt(dp[0] * dp[0] + dp[1] * dp[1] + dp[2] * dp[2]) + 1;
                dist[1] = sqrt(dp[3] * dp[3] + dp[4] * dp[4] + dp[5] * dp[5]) + 1;
                dist[2] = sqrt(dp[6] * dp[6] + dp[7] * dp[7] + dp[8] * dp[8]) + 1;
                dist[3] = sqrt(dp[9] * dp[9] + dp[10] * dp[10] + dp[11] * dp[11]) + 1;
                
        
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
        
                a[0] = (G * mass[j]) / (dist[0] * dist[0] * dist[0] * EPS);
                a[1] = (G * mass[j + 1]) / (dist[1] * dist[1] * dist[1] * EPS);
                a[2] = (G * mass[j + 2]) / (dist[2] * dist[2] * dist[2] * EPS);
                a[3] = (G * mass[j + 3]) / (dist[3] * dist[3] * dist[3] * EPS);
                
        
                ap[0] += a[0] * dp[0]; /* accumulate the acceleration from gravitational attraction */
                ap[1] += a[0] * dp[1];
                ap[2] += a[0] * dp[2];
                
                ap[3] += a[1] * dp[3]; /* accumulate the acceleration from gravitational attraction */
                ap[4] += a[1] * dp[4];
                ap[5] += a[1] * dp[5];
                
                ap[6] += a[2] * dp[6]; /* accumulate the acceleration from gravitational attraction */
                ap[7] += a[2] * dp[7];
                ap[8] += a[2] * dp[8];
                
                ap[9] += a[3] * dp[9]; /* accumulate the acceleration from gravitational attraction */
                ap[10] += a[3] * dp[10];
                ap[11] += a[3] * dp[11];
        
                xva[i] += dt * ap[0]; /* update velocity of particle "i" */
                yva[i] += dt * ap[1];
                zva[i] += dt * ap[2];
                
                xva[i] += dt * ap[3]; /* update velocity of particle "i" */
                yva[i] += dt * ap[4];
                zva[i] += dt * ap[5];
                
                xva[i] += dt * ap[6]; /* update velocity of particle "i" */
                yva[i] += dt * ap[7];
                zva[i] += dt * ap[8];
                
                xva[i] += dt * ap[9]; /* update velocity of particle "i" */
                yva[i] += dt * ap[10];
                zva[i] += dt * ap[11];
            }
    
            xb[i] = xa[i] + dt * xva[i]; /* update position of particle "i" */
            yb[i] = ya[i] + dt * yva[i];
            zb[i] = za[i] + dt * zva[i];  
        }
        
        free(dist);
        free(a);
        free(dp);
        free(ap);
            
        }
        
        end = MPI_Wtime();
        
        printf("Iteration %d:\t%.10f seconds\t%.10f seconds\n", frame, end - start, end_writing - start_writing);
        total_frame_time += end - start;
        
        swap_dubs(&xa, &xb);
        swap_dubs(&ya, &yb);
        swap_dubs(&za, &zb);
        
        swap_dubs(&xva, &xvb);
        swap_dubs(&yva, &yvb);
        swap_dubs(&zva, &zvb);
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