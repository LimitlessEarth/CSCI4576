/* 
 * HPGM helper functions to be included in main
 * Provided by Michael Oberg, Modified by Adam Ross
 * 
 */

// System includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <math.h>

// User includes
#include "globals.h"
#include "pprintf.h"
#include "helper.h"

typedef enum { false, true } bool; // Provide C++ style 'bool' type in C

bool readpgm( char *filename ){
    // Read a PGM file into the local task
    //
    // Input: char *filename, name of file to read
    // Returns: True if file read successfully, False otherwise
    //
    // Preconditions:
    //  * global variables nrows, ncols, my_row, my_col must be set
    //
    // Side effects:
    //  * sets global variables local_width, local_height to local game size
    //  * sets global variables field_width, field_height to local field size
    //  * allocates global variables env_a and env_b
    int             x = 0;
    int             y = 0;
    int             start_x, start_y;
    int             b, lx, ly, ll;
    char            header[10];
    int             depth;
    int             rv;
    int             grab_width;
    int             grab_height;
    int             x_add = 1;
    int             y_add = 1;

    pp_set_banner( "pgm:readpgm" );

    // Open the file
    if (rank == 0)
        pprintf( "Opening file %s\n", filename );
    FILE *fp = fopen( filename, "r" );
    if (!fp) {
        pprintf( "Error: The file '%s' could not be opened.\n", filename );
        return false;
    }

    // Read the PGM header, which looks like this:
    //  |P5        magic version number
    //  |900 900       width height
    //  |255         depth
    rv = fscanf( fp, "%6s\n%i %i\n%i\n", header, &global_width, &global_height, &depth );
    if (rv != 4){
        if (rank == 0) 
              pprintf( "Error: The file '%s' did not have a valid PGM header\n", filename );
        return false;
    }
    if (rank == 0)
        pprintf( "%s: %s %i %i %i\n", filename, header, global_width, global_height, depth );

    // Make sure the header is valid
    if (strcmp( header, "P5")) {
        if(rank==0) 
            pprintf( "Error: PGM file is not a valid P5 pixmap.\n" );
        return false;
    }
    if (depth != 255) {
        if (rank == 0) 
            pprintf( "Error: PGM file has depth=%i, require depth=255 \n", depth );
        return false;
    }

    // Make sure that the width and height are divisible by the number of
    // processors in x and y directions

    if (global_width % ncols) {
        if (rank == 0)
            pprintf( "Error: %i pixel width cannot be divided into %i cols\n", global_width, ncols );
        return false;
    }
    if (global_height % nrows) {
        if (rank == 0)
            pprintf( "Error: %i pixel height cannot be divided into %i rows\n", global_height, nrows );
        return false;
    }

    // Divide the total image among the local processors
    local_width = global_width / ncols;
    local_height = global_height / nrows;

    // Find out where my starting range is
    start_x = local_width * my_col;
    start_y = local_height * my_row;
    
    grab_width = local_width;
    grab_height = local_height;

    pprintf( "Hosting data for x:%03i-%03i y:%03i-%03i\n", 
        start_x, start_x + local_width,
        start_y, start_y + local_height );

    // Create the array!
    field_width = local_width + 2;
    field_height = local_height + 2;
    
    // allocate contiguous memory - returns a pointer to the memory
    env_a = Allocate_Square_Matrix(field_width, field_height);
    env_b = Allocate_Square_Matrix(field_width, field_height);
    
    // Need to handle edge cases to not grab extras
     if (dist_type == ROW ){
        grab_height = field_height;
        
        if (rank == 0) {
            grab_height--;
        } else if (rank == np - 1) {
            grab_height--;
            start_y--;
            y_add = 0;
        } else {
            start_y--;
            y_add = 0;
        }
    } else if (dist_type == GRID) {
        grab_width = field_width;
        grab_height = field_height;
        
        if (my_row == 0) {
            grab_height--;
        } else if (my_row == sqrt(np) - 1) {
            grab_height--;
            start_y--;
            y_add = 0;
        } else {
            start_y--;
            y_add = 0;
        }
        
        if (my_col == 0) {
            grab_width--;
        } else if (my_col == sqrt(np) - 1) {
            grab_width--;
            start_x--;
            x_add = 0;
        } else {
            start_x--;
            x_add = 0;
        }
    }
    
    //pprintf("start_x: %d\tstart_y: %d\tx_add: %d\ty_add: %d\t\n", start_x, start_y, x_add, y_add);
    //pprintf("grab_width: %d\tgrab_height: %d\t\n", grab_width, grab_height);
    
    // Read the data from the file. Save the local data to the local array.
    for (y = 0; y < global_height; y++) {
        for (x = 0; x < global_width; x++) {
            // Read the next character
            b = fgetc(fp);
            if (b == EOF){
                pprintf( "Error: Encountered EOF at [%i,%i]\n", y,x );
                return false;
            }

            // From the PGM, black cells (b=0) are bugs, all other 
            // cells are background 
            if (b == 0) {
                b = 1;
            } else {
                b = 0;
            }

            // If the character is local, then save it!
            if (x >= start_x &&
                x < start_x + grab_width && 
                y >= start_y && 
                y < start_y + grab_height) {
                    
                // Calculate the local pixels (+1 for ghost row,col)
                lx = x - start_x + x_add;
                ly = y - start_y + y_add;
                ll = (ly * field_width + lx);
                env_a[ll] = b;
                env_b[ll] = b;
            } // save local point


        } // for x
    } // for y
    


    fclose(fp);

    pp_reset_banner();
    return true;
}
