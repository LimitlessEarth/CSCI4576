/* MT1 - Midterm Part I: Conway's Game of Line 
 * 
 * 
 * Name: Adam Ross
 *
 * Input: -i filename, -d distribution type <0 - serial, 1 - row, 2 - grid> 
 *        -s turn on asynchronous MPI functions, -c <#> if and when to count living 
 * Output: Various runtime information including bug counting if turned on
 *
 * 
 * Note: a Much of this code, namely the pgm reader and most of the support libraries
 * is credited to: Dr. Matthew Woitaszek
 * 
 * Written by Adam Ross, modified from code supplied by Michael Oberg, modified from code supplied by Dr. Matthew Woitaszek
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
    int                 top_dest, 
                        top_source, 
                        bot_dest , 
                        bot_source,
                        left_dest, 
                        left_source, 
                        right_dest, 
                        right_source =     5280;                
    MPI_Status          status;
    MPI_Request         ar, br, lr, rr;
    MPI_File            out_file;
    int                 counting =          -1;
    int                 count =             0;
    int                 total =             0;
    int                 n =                 0;
    int                 option =            -1;
    dist                dist_type;
    bool                async =             false;
    int                 iter_num =          1000;
    char                *filename;
    char                frame[47];
    int                 gsizes[2], distribs[2], dargs[2], psizes[2];
    MPI_Datatype        ext_array;
    MPI_Datatype        darray;
    MPI_Datatype        column;
    
        
    // Parse commandline
    while ((option = getopt(argc, argv, "d:sn:c:i:")) != -1) {        
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
    if (dist_type < 2) {
        if (!rank)
            pprintf("Row or Serial distribution selected.\n");
        ncols = 1;
        nrows = np;
        my_col = 0;
        my_row = rank;
    } else {
        if (!rank)
            pprintf("Grid distribution selected.\n");
        nrows = (int)sqrt(np);
        ncols = (int)sqrt(np);
        my_row = rank / nrows;
        my_col = rank - my_row * nrows;
        
        //pprintf("Num rows%d\tNum cols %d\tMy row %d\tMy col %d\n", nrows, ncols, my_row, my_col);
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
    
    // Set up darray create properties
    gsizes[0] = global_height; /* no. of rows in global array */
    gsizes[1] = global_width; /* no. of columns in global array*/
    distribs[0] = MPI_DISTRIBUTE_BLOCK;
    distribs[1] = MPI_DISTRIBUTE_BLOCK;
    dargs[0] = MPI_DISTRIBUTE_DFLT_DARG;
    dargs[1] = MPI_DISTRIBUTE_DFLT_DARG;
    psizes[0] = nrows; /* no. of processes in vertical dimension of process grid */
    psizes[1] = ncols; /* no. of processes in horizontal dimension of process grid */
    
    // Create darray and commit
    MPI_Type_create_darray(np, rank, 2, gsizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_UNSIGNED_CHAR, &darray);
    MPI_Type_commit(&darray);
    
    MPI_Type_vector(local_height, local_width, field_width, MPI_UNSIGNED_CHAR, &ext_array);
    MPI_Type_commit(&ext_array);
        
    // Build MPI datatype vector of every Nth item - i.e. a column
    MPI_Type_vector(local_height, 1, field_width, MPI_UNSIGNED_CHAR, &column);
    MPI_Type_commit(&column);
    
    // allocate memory to print whole stages into pgm files for animation
    if (rank == 0) {
        out_buffer = Allocate_Square_Matrix(awidth, aheight); 
    }
    
    // Count initial living count
    if (counting != -1) {
        count = count_alive(env_a);
        pprintf("Bugs alive at the start: %d\n", count);
        
        MPI_Allreduce(&count, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (rank == 0) {
            pprintf("%i total bugs alive at the start.\n", total);
        }
    }
    
    // Perform initial exhange to calculate 0 and 1 states
    if (async && dist_type >= 1) {
        if (rank == 0) {
            pprintf("Asynchronous communication starting\n");
        }
        if (dist_type == 1) {
            top_dest = bot_source = rank - 1;              
            top_source = bot_dest = rank + 1;
        
            if (!rank) { // rank 0, no need to send
                top_dest = MPI_PROC_NULL;
                bot_source = MPI_PROC_NULL;
            } else if (rank == (np - 1)) { // rank np-1 no need to send
                top_source = MPI_PROC_NULL;
                bot_dest = MPI_PROC_NULL;
            }
        } else if (dist_type == 2) {
        // calculate pairings
            top_dest = bot_source = rank - nrows;              
            top_source = bot_dest = rank + nrows;
            left_dest = right_source = rank - 1;
            left_source = right_dest = rank + 1;
                    
            if (my_row == 0) { // top row no need to send up 
                top_dest = MPI_PROC_NULL;
                bot_source = MPI_PROC_NULL;
            } else if (my_row == sqrt(np) - 1) { // rank bottom row no need to send down
                top_source = MPI_PROC_NULL;
                bot_dest = MPI_PROC_NULL;
            }
            if (my_col == 0) {
                left_dest = MPI_PROC_NULL;
                right_source = MPI_PROC_NULL;
            } else if (my_col == sqrt(np) - 1) {
                left_source = MPI_PROC_NULL;
                right_dest = MPI_PROC_NULL;
            }
            //pprintf("top: %d\tbot %d\tleft %d\tright %d\tProc %d\n", top_dest, bot_dest, left_dest, right_dest, MPI_PROC_NULL);
        }
        
        if (dist_type == 2) {
            MPI_Isend(&env_a[1 * field_width + field_width - 2], 1, column, left_dest, 0, MPI_COMM_WORLD, &lr);
            MPI_Isend(&env_a[1 * field_width + 1], 1, column, right_dest, 0, MPI_COMM_WORLD, &rr);
            
            MPI_Irecv(&env_a[1 * field_width + 0], 1, column, left_source, 0, MPI_COMM_WORLD, &lr);
            MPI_Irecv(&env_a[1 * field_width + field_width - 1], 1, column, right_source, 0, MPI_COMM_WORLD, &rr);
            // Need the horizontal data before we send vertically
            MPI_Wait(&lr, &status);
            MPI_Wait(&rr, &status);
        }
        
        MPI_Isend(&env_a[1 * field_width + 0], field_width, MPI_UNSIGNED_CHAR, top_dest, 0, MPI_COMM_WORLD, &ar);
        MPI_Isend(&env_a[(field_height - 2) * field_width + 0], field_width, MPI_UNSIGNED_CHAR, bot_dest, 0, MPI_COMM_WORLD, &br);        
    }
        
    while(n < iter_num) {
        // sync or a async here MPI_PROC_NULs
        if (dist_type > 0) {
            // calculate pairings
            if (dist_type == 1) { // row distro
                top_dest = bot_source = rank - 1;              
                top_source = bot_dest = rank + 1;
            
                if (rank == 0) { // rank 0, no need to send
                    top_dest = MPI_PROC_NULL;
                    bot_source = MPI_PROC_NULL;
                } else if (rank == (np - 1)) { // rank np-1 no need to send
                    top_source = MPI_PROC_NULL;
                    bot_dest = MPI_PROC_NULL;
                }
            } else if (dist_type == 2) {
            // calculate pairings
                top_dest = bot_source = rank - nrows;              
                top_source = bot_dest = rank + nrows;
                left_dest = right_source = rank - 1;
                left_source = right_dest = rank + 1;
                        
                if (my_row == 0) { // top row no need to send up 
                    top_dest = MPI_PROC_NULL;
                    bot_source = MPI_PROC_NULL;
                } else if (my_row == sqrt(np) - 1) { // rank bottom row no need to send down
                    top_source = MPI_PROC_NULL;
                    bot_dest = MPI_PROC_NULL;
                }
                if (my_col == 0) {
                    left_dest = MPI_PROC_NULL;
                    right_source = MPI_PROC_NULL;
                } else if (my_col == sqrt(np) - 1) {
                    left_source = MPI_PROC_NULL;
                    right_dest = MPI_PROC_NULL;
                }
                //pprintf("top: %d\tbot %d\tleft %d\tright %d\tProc %d\n", top_dest, bot_dest, left_dest, right_dest, MPI_PROC_NULL);
            }
            
            if (!async) {
                if (dist_type == 2) {
                    // Send to right or recv from left
                    MPI_Sendrecv(&env_a[1 * field_width + field_width - 2], 1, column, left_dest, 0,
                                 &env_a[1 * field_width + 0], 1, column, left_source, 0, MPI_COMM_WORLD, &status);
                    // Send to left or recv from right
                    MPI_Sendrecv(&env_a[1 * field_width + 1], 1, column, right_dest, 0,
                                 &env_a[1 * field_width + field_width - 1], 1, column, right_source, 0, MPI_COMM_WORLD, &status);
                } 
                // Send to below or recv from above
                MPI_Sendrecv(&env_a[1 * field_width + 0], field_width, MPI_UNSIGNED_CHAR, top_dest, 0,
                             &env_a[(field_height - 1) * field_width + 0], field_width, MPI_UNSIGNED_CHAR, top_source, 0, MPI_COMM_WORLD, &status);
                // Send to above or recv from below
                MPI_Sendrecv(&env_a[(field_height - 2) * field_width + 0], field_width, MPI_UNSIGNED_CHAR, bot_dest, 0,
                             &env_a[0 * field_width + 0], field_width, MPI_UNSIGNED_CHAR, bot_source, 0, MPI_COMM_WORLD, &status);
                
            } else { // Aschrnous enabled, receive from the last iteration or inital setup
                MPI_Irecv(&env_a[(field_height - 1) * field_width + 0], field_width, MPI_UNSIGNED_CHAR, top_source, 0, MPI_COMM_WORLD, &ar);
                MPI_Irecv(&env_a[0 * field_width + 0], field_width, MPI_UNSIGNED_CHAR, bot_source, 0, MPI_COMM_WORLD, &br);
                // To avoid getting data mixed up wait for it to come through
                MPI_Wait(&ar, &status);
                MPI_Wait(&br, &status);
            }
        } 
        
        // calulate neighbors and form state + 1
        for (i = 1; i < local_height + 1; i++) {
            for (j = 1; j < local_width + 1; j++) {
                neighbors = 0;
                // loop unroll neighbor checking - access row dominant
                neighbors += env_a[(i - 1) * field_width + j - 1] + env_a[(i - 1) * field_width + j] + env_a[(i - 1) * field_width + j + 1]; 
                neighbors += env_a[i * field_width + j - 1] +                                          env_a[i * field_width + j + 1];
                neighbors += env_a[(i + 1) * field_width + j - 1] + env_a[(i + 1) * field_width + j] + env_a[(i + 1) * field_width + j + 1];

                // Determine env_b based on neighbors in env_a
                if (neighbors == 2) {
                    env_b[i * field_width + j] = env_a[i * field_width + j]; // exactly 2 spawn
                } else if (neighbors == 3) {
                    env_b[i * field_width + j] = 1; // exactly 3 spawn
                } else {
                    env_b[i * field_width + j] = 0; // zero or one or 4 or more die                    
                }
            }
        }
        
        // If we are doing async we now have the data we need for the next iter, send it
        if (async && dist_type == 1) {
            MPI_Isend(&env_b[1 * field_width + 0], field_width, MPI_UNSIGNED_CHAR, top_dest, 0, MPI_COMM_WORLD, &ar);
            MPI_Isend(&env_b[(field_height - 2) * field_width + 0], field_width, MPI_UNSIGNED_CHAR, bot_dest, 0, MPI_COMM_WORLD, &br);
        } else if (async && dist_type == 2) {
            MPI_Isend(&env_b[1 * field_width + field_width - 2], 1, column, left_dest, 0, MPI_COMM_WORLD, &lr);
            MPI_Isend(&env_b[1 * field_width + 1], 1, column, right_dest, 0, MPI_COMM_WORLD, &rr);
        }
        
        
        for (int k = 1; k < field_height - 1; k++) {
            for (int a = 1; a < field_width - 1; a++) {                    
                if (!env_b[k * field_width + a]) {
                    env_a[k * field_width + a] = 255;
                } else {
                    env_a[k * field_width + a] = 0;
                }
            }
        }
                
        sprintf(frame, "/oasis/scratch/comet/adamross/temp_project/%d.pgm", n);
        MPI_File_open(MPI_COMM_WORLD, frame, MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &out_file);
        
        char header[15];
        sprintf(header, "P5\n%d %d\n%d\n", global_width, global_height, 255);
        int header_len = strlen(header);
            
        //write header
        MPI_File_set_view(out_file, 0,  MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, "native", MPI_INFO_NULL);
        MPI_File_write(out_file, &header, 13, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);   

        // write data
        //MPI_File_set_view(out_file, 15 + rank * local_width + local_width, MPI_UNSIGNED_CHAR, darray, "native", MPI_INFO_NULL);
        MPI_File_set_view(out_file, 13, MPI_UNSIGNED_CHAR, darray, "native", MPI_INFO_NULL);
        
        //MPI_File_write(out_file, env_a, (local_height * local_width), ext_array, &status);
        MPI_File_write(out_file, &env_a[field_width + 1], 1, ext_array, &status);
        MPI_File_close(&out_file);
        
        for (int k = 1; k < field_height - 1; k++) {
            for (int a = 1; a < field_width  - 1; a++) {                    
                if (!env_a[k * field_width + a]) {
                    env_a[k * field_width + a] = 0;
                } else {
                    env_a[k * field_width + a] = 1;
                }
            }
        }
        
        
        // Uncomment to produce pgm files per frame
        /*MPI_Gather(env_b, field_width * field_height, MPI_UNSIGNED_CHAR, out_buffer, field_width * field_height, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
        
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
        
        // If counting is turned on print living bugs this iteration
        if (n != 0 && (n % counting) == 0) {
            count = count_alive(env_b);
            
            MPI_Allreduce(&count, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
            if (rank == 0) {
                pprintf("%i total bugs alive at iteraion %d\n", total, n);
            }
        }
        
        if (async && dist_type == 2) {
            MPI_Irecv(&env_b[1 * field_width + 0], 1, column, left_source, 0, MPI_COMM_WORLD, &lr);
            MPI_Irecv(&env_b[1 * field_width + field_width - 1], 1, column, right_source, 0, MPI_COMM_WORLD, &rr);
            // Need the horizontal data before we send vertically
            MPI_Wait(&lr, &status);
            MPI_Wait(&rr, &status);
        
            MPI_Isend(&env_b[1 * field_width + 0], field_width, MPI_UNSIGNED_CHAR, top_dest, 0, MPI_COMM_WORLD, &ar);
            MPI_Isend(&env_b[(field_height - 2) * field_width + 0], field_width, MPI_UNSIGNED_CHAR, bot_dest, 0, MPI_COMM_WORLD, &br);
        }
        
        n++;
        swap(&env_b, &env_a);        
    }
    
    // Final living count
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
