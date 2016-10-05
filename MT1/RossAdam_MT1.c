/* HW6 Dense Matrix Multiplication Serial 
 * 
 * 
 * Name: Adam Ross
 *
 * Input: none
 * Output: Printed Matricies to show correctness
 *
 * 
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <string.h>
#include "mpi.h"

// Include global variables. Only this file needs the #define
#define __MAIN 
#include "globals.h"
#undef __MAIN

// User includes
#include "pprintf.h"
#include "pgm.h"
#include "helper.h"

typedef enum { SERIAL, ROW, BLOCK } dist;

int main(int argc, char* argv[]) {
    unsigned short      i, j;
    unsigned short      neighbors =         0;
    int                 top_dest =          5280;
    int                 top_source =        5280;
    int                 bot_dest =          5280;
    int                 bot_source =        5280;
    MPI_Status          status;
    MPI_Request         rq, qr;
    int                 counting =          -1;
    int                 count =             0;
    int                 total =             0;
    int                 n =                 0;
    int                 option =            -1;
    dist                dist_type;
    bool                async =             false;
    int                 iter_num =          1000;
    char                *filename;
    char                *out_filename;
    char                frame[47];
        
    
    while ((option = getopt(argc, argv, "d:sn:c:i:o:")) != -1) {        
        switch (option) {
             case 'd' : 
                 dist_type = atoi(optarg);
                 break;
             case 's' : 
                 async = true;
                 break;
             case 'n' : 
                 iter_num = atoi(optarg);
                 break;
             case 'c' : 
                 counting = atoi(optarg);
                 break;
             case 'i' :
                 filename = optarg;
                 break;
             case 'o' :
                 out_filename = optarg;
                 break;
             default:
                 print_usage(); 
                 exit(1);
        }
    }
        
    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get the communicator and process information
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // Print rank and hostname
    MPI_Get_processor_name(my_name, &my_name_len);
    printf("Rank %i is running on %s\n", rank, my_name );
    
    // Initialize the pretty printer
    init_pprintf(rank);
    pp_set_banner("main");    
    
    if (rank == 0) {        
        pprintf("Welcome to Conway's Game of Life!\n");
    }

    //
    // Determine the partitioning
    //
    if (!dist_type || dist_type == 1) {
        if (!rank)
            pprintf("Row distribution selected.\n");
        ncols = 1;
        nrows = np;
        my_col = 0;
        my_row = rank;
    }
    
    if (np != nrows * ncols) {
        if (!rank)
             pprintf("Error: %ix%i partitioning requires %i np (%i provided)\n", 
                  nrows, ncols, nrows * ncols, np );
        MPI_Finalize();
        return 1;
    }

    
    // Now, calculate neighbors (N, S, E, W, NW, NE, SW, SE)  
    // ... which means you ...


    // Read the PGM file. The readpgm() routine reads the PGM file and, based
    // on the previously set nrows, ncols, my_row, and my_col variables, loads
    // just the local part of the field onto the current processor. The
    // variables local_width, local_height, field_width, field_height, as well
    // as the fields (field_a, field_b) are allocated and filled.
    if (!readpgm(filename)) {
        if (rank == 0)
            pprintf("An error occured while reading the pgm file\n");
        MPI_Finalize();
        return 1;
    }
    
    if (counting != -1) {
        count = count_alive(env_a);
        pprintf("Bugs alive at the start: %d\n", count);
        
        MPI_Allreduce(&count, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (rank == 0) {
            pprintf("%i total bugs alive at the start.\n", total);
        }
    }
        
    while(n < iter_num) {
        /*MPI_Gather(env_a, field_width * field_height, MPI_CHAR, out_buffer, field_width * field_height, MPI_CHAR, 0, MPI_COMM_WORLD);
        
        if (rank == 0) {
           for (int k = 0; k < aheight; k++) {
               for (int a = 0; a < awidth; a++) {                    
                   if (!out_buffer[k * awidth + a]) {
                       out_buffer[k * awidth + a] = 255;
                   } else {
                       out_buffer[k * awidth + a] = 0;
                   }
               }
           }

           sprintf(frame, "%d.pgm", n);
           FILE *file = fopen(frame, "w");
           fprintf(file, "P5\n");
           fprintf(file, "%d %d\n", awidth, aheight);
           fprintf(file, "%d\n", 255);
           fwrite(out_buffer, sizeof(unsigned char), awidth * aheight, file);
           fclose(file);
       }*/
        
        // sync or a async here MPI_PROC_NULs
        if (dist_type == 1) { // row distro
            // above padding
            top_dest = bot_source = rank - 1;              
            top_source = bot_dest = rank + 1;
            if (!rank) {
                top_dest = MPI_PROC_NULL;
                bot_source = MPI_PROC_NULL;
            } else if (rank == (np - 1)) {
                top_source = MPI_PROC_NULL;  
                bot_dest = MPI_PROC_NULL;                    
            }
            
            if (!async) {
                // Send to below or recv from above
                MPI_Sendrecv(&env_a[1 * field_width + 0], field_width, MPI_UNSIGNED_CHAR, top_dest, 0,
                             &env_a[(field_height - 1) * field_width + 0], field_width, MPI_UNSIGNED_CHAR, top_source, 0, MPI_COMM_WORLD, &status);
                // Send to above or recv from below
                MPI_Sendrecv(&env_a[(field_height - 2) * field_width + 0], field_width, MPI_UNSIGNED_CHAR, bot_dest, 0,
                             &env_a[0 * field_width + 0], field_width, MPI_UNSIGNED_CHAR, bot_source, 0, MPI_COMM_WORLD, &status);
                
            } else {
                MPI_Irecv(&env_a[(field_height - 1) * field_width + 0], field_width, MPI_CHAR, top_source, 0, MPI_COMM_WORLD, &rq);
                MPI_Irecv(&env_a[0 * field_width + 0], field_width, MPI_CHAR, bot_source, 0, MPI_COMM_WORLD, &qr);
            }
        } // else block distro
        
        // calulate neighbors and form state + 1
        for (i = 1; i < local_height + 1; i++) {
            for (j = 1; j < local_width + 1; j++) {
                neighbors = 0;
                // loop unroll neighbor checking - access row dominant
                neighbors += env_a[(i - 1) * field_width + j - 1] + env_a[(i - 1) * field_width + j] + env_a[(i - 1) * field_width + j + 1]; 
                neighbors += env_a[i * field_width + j - 1] +                                          env_a[i * field_width + j + 1];
                neighbors += env_a[(i + 1) * field_width + j - 1] + env_a[(i + 1) * field_width + j] + env_a[(i + 1) * field_width + j + 1];

                if (neighbors == 2) {
                    env_b[i * field_width + j] = env_a[i * field_width + j]; // exactly 2 spawn
                } else if (neighbors == 3) {
                    env_b[i * field_width + j] = 1; // exactly 3 spawn
                } else {
                    env_b[i * field_width + j] = 0; // zero or one or 4 or more die                    
                }
            }
        }
        if (async && dist_type == 1) {
            MPI_Isend(&env_a[1 * field_width + 0], field_width, MPI_CHAR, top_dest, 0, MPI_COMM_WORLD, &rq);
            MPI_Isend(&env_a[(field_height - 2) * field_width + 0], field_width, MPI_CHAR, bot_dest, 0, MPI_COMM_WORLD, &qr);
        }
        
        
        if (n != 0 && (n % counting) == 0) {
            count = count_alive(env_a);
            
            MPI_Allreduce(&count, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
            if (rank == 0) {
                pprintf("%i total bugs alive at iteraion %d\n", total, n);
            }
        }
        
        n++;
        swap(&env_b, &env_a);        
    }
    
    if (counting != -1 && n != counting) {
        count = count_alive(env_a);
        pprintf("Per process bugs alive at the end: %d\n", count);
        
        MPI_Allreduce(&count, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (rank == 0) {
            pprintf("%i total bugs alive at the end.\n", total);
        }
    }
    
    // Free the fields
    MPI_Barrier(MPI_COMM_WORLD);
    if (env_a != NULL) free( env_a );
    if (env_b != NULL) free( env_b );
    
    MPI_Finalize();
    
    
} /* end main */
