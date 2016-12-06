#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include <math.h>
#include <time.h>
#include "pprintf.h"
#include <getopt.h>

// Self explanitory
void print_usage() {
    printf("Usage: -d image dimensions\n-t time step\n-i number of iterations\n-e particle init type\n-w writing on off\n-p number of particles\n-T number of threads(if applicable)\n");
}

/* 
 * Helper method to swap the matrix pointers
 * Input: two Particle matricies
 */
void swap(Particle **a, Particle **b) {
    Particle              *tmp = *a;
    *a = *b;
    *b = tmp;
}

/* 
 * Helper method to swap the matrix pointers
 * Input: two Particle matricies
 */
void swap_pass(Pass_Particle **a, Pass_Particle **b) {
    Pass_Particle              *tmp = *a;
    *a = *b;
    *b = tmp;
}

/* 
 * Helper method to set up the global defaults
 * Input: None
 */
void globals_init() {
    img_dim = 1500;
    img_len = img_dim * img_dim;
    num_part = 1000;
    num_iter = 10;
    dt = 4000000;
    np = -666;
    init_type = INIT_LINEAR;
    writing = false;
    super_comp = false;
    num_threads = -1;
}

/* 
 * Helper method to parse command line args
 * Input: argc and argv
 */
void parse_args(int argc, char* argv[]) {
    int                 option =            -1;
    
    // Parse commandline
    while ((option = getopt(argc, argv, "d:t:i:e:wp:sT:")) != -1) {        
        switch (option) {
             case 'd' : 
                 img_dim = atoi(optarg);
                 break;
             case 't' : 
                 dt = atoi(optarg) * 10000;
                 break;
             case 'i' : 
                 num_iter = atoi(optarg);
                 break;
             case 'e' : 
                 init_type = atoi(optarg);
                 break;
             case 'w' :
                 writing = true;
                 break;
             case 'p' :
                 num_part = atoi(optarg);
                 break;
             case 's' :
                 super_comp = true;
                 break;
             case 'T' : 
                 num_threads = atoi(optarg);
                 break;
             default:
                 print_usage(); 
                 exit(1);
        }
    }
}

/* 
 * Helper method to initalize the Particle data
 * Input: None
 */
void initialize_particles() {
    int                 i, j, num;
    
    if (np != -666 && np != 1) {
        num = my_num_part;
    } else {
        num = num_part;
        rank = 0;
    }
    
    srand(time(NULL));
    
    switch (init_type) {
        case INIT_LINEAR:
            printf("Initalizing particles linear on %d to %d\n", rank * num, (rank + 1) * num);
            for (i = 0; i < num; i++) {
                Particles_a[i].mass = MASS_MAX * (double)(rand() / ((double)RAND_MAX + 1.0));
        
                Particles_a[i].pos[X] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
                Particles_a[i].pos[Y] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
                Particles_a[i].pos[Z] = DOMAIN_SIZE/4 * (double)(rand() / ((double)RAND_MAX + 1.0)) + 3*DOMAIN_SIZE/8;
            }
            break;
        case INIT_SPIRAL:
            printf("Initalizing particles sipral on %d to %d\n", rank * num, (rank + 1) * num);
            for (i = 0; i < num; i++) {
                j = rank * num + i;
                Particles_a[i].mass = MASS_MAX * (double)(rand() / ((double)RAND_MAX + 1.0));
                
                Particles_a[i].pos[X] = ((1+j/num_part) * cos(2*M_PI*j/num_part) / 2) * DOMAIN_SIZE/4 * 
                                        (double)(rand() / ((double)RAND_MAX + 1.0)) + DOMAIN_SIZE/2;
                Particles_a[i].pos[Y] = ((1+j/num_part) * sin(2*M_PI*j/num_part) / 2) * DOMAIN_SIZE/4 * 
                                        (double)(rand() / ((double)RAND_MAX + 1.0)) + DOMAIN_SIZE/2;
                Particles_a[i].pos[Z] = DOMAIN_SIZE/8 * (double)(rand() / ((double)RAND_MAX + 1.0)) + DOMAIN_SIZE/2;
                
                Particles_a[i].vel[Y] = ((1+j/num_part) * cos(2*M_PI*j/num_part) / 2) * 3000000000000000 * 
                                        (double)(rand() / ((double)RAND_MAX + 1.0));
                Particles_a[i].vel[X] = -((1+j/num_part) * sin(2*M_PI*j/num_part) / 2) * 3000000000000000 * 
                                        (double)(rand() / ((double)RAND_MAX + 1.0));
            }
            break;
    }
}

/* 
 * Helper method to set the passing Particle data
 * Input: None
 */
void set_pass_particles() {
    int                 i;
    
    if (Particles_pass_a != NULL && np != 1) {
        for (i = 0; i < my_num_part; i++) {
            Particles_pass_a[i].mass = Particles_a[i].mass;              
            
            Particles_pass_a[i].pos[X] = Particles_a[i].pos[X];
            Particles_pass_a[i].pos[Y] = Particles_a[i].pos[Y];         
            Particles_pass_a[i].pos[Z] = Particles_a[i].pos[Z];
                    
        }
    }
}

/* 
 * Helper method to write out the Particle data
 * Input: The current frame
 */
void write_data_serial(int n) {
    int                 loc, x, y, a;
    char                frame[47];
    
    for (a = 0; a < num_part; a++) {        
        x = (Particles_a[a].pos[X] / DOMAIN_SIZE) * img_dim;        
        y = (Particles_a[a].pos[Y] / DOMAIN_SIZE) * img_dim;
        
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
        x = (Particles_a[a].pos[X] / DOMAIN_SIZE) * img_dim;        
        y = (Particles_a[a].pos[Y] / DOMAIN_SIZE) * img_dim;
                
        if (x < 0 || x > img_dim)
            continue;
                
        loc = x + (img_dim * y);
        if (loc >= 0 && loc < img_len) {        
            out_buffer[loc] = 0;
        }    
    }
}

/* 
 * Helper method to write out the Particle data
 * Input: The current frame
 */
void write_data_parallel(int n) {
    int                 loc, x, y, a;
    char                frame[47];
    
    for (a = 0; a < num_part; a++) {        
        x = (Particles_out[a].pos[X] / DOMAIN_SIZE) * img_dim;        
        y = (Particles_out[a].pos[Y] / DOMAIN_SIZE) * img_dim;
        
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
        x = (Particles_out[a].pos[X] / DOMAIN_SIZE) * img_dim;        
        y = (Particles_out[a].pos[Y] / DOMAIN_SIZE) * img_dim;
        
        if (x < 0 || x > img_dim)
            continue;
        
        loc = x + (img_dim * y);
        if (loc >= 0 && loc < img_len) {        
            out_buffer[loc] = 0;
        }    
    }
}

/* Helper function calculate the confidence interval, error margins and determine 
 * if we should keep looping. 
 * Returns 1 or 0 for conintue or stop.
*/
int Calc_Confidence_Interval_stop(double *timing_data, int n) {
    double              sum =               0.0;
    double              mean =              0.0;
    double              std_dev =           0.0;
    double              marg_err =          0.0;
    double              marg_perc =         100.0;
    int                 i;
    
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
        return 0;
    }
    if (marg_perc > 5.0  && n < 20) {
        return 0;
    } else {
        printf("%d\t%1.20f\t%1.10f\t%1.10f\t%f\t", n, mean, std_dev, marg_err, marg_perc);        
        return 1;
    }
}