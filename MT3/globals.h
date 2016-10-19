// Conway's Game of Life
// Global variable include file
//
// CSCI 4576/5576 High Performance Scientific Computing
// Matthew Woitaszek

// <soapbox>
// This file contains global variables: variables that are defined throughout
// the entire program, even between multiple independent source files. Of
// course, global variables are generally bad, but they're useful here because
// it allows all of the source files to know their rank and the number of MPI
// tasks. But don't use it lightly.
//
// How it works:
//  * One .cpp file -- usually the one that contains main(), includes this file
//    within #define __MAIN, like this:
//      #define __MAIN
//      #include globals.h
//      #undef __MAIN
//  * The other files just "#include globals.h"  

typedef enum { SERIAL, ROW, GRID } dist;

#ifdef __MAIN
int                     rank;
int                     np;
int                     my_name_len;
char                    my_name[255];
#else
extern int              rank;
extern int              np;
extern int              my_name_len;
extern char             *my_name;
#endif

//
// Conway globals
//
#ifdef __MAIN

int                     nrows;          // Number of rows in our partitioning
int                     ncols;          // Number of columns in our partitioning
int                     my_row;         // My row number
int                     my_col;         // My column number

// Local logical game size
int                     local_width;    // Width and height of game on this processor
int                     local_height;
int                     global_width;
int                     global_height;
int                     N;

// Local physical field size
int                     field_width;        // Width and height of field on this processor
int                     field_height;       // (should be local_width+2, local_height+2)
unsigned char           *env_a;             // 1D character array to represent our 1st 2D environment
unsigned char           *env_b;             // 1D character array to represent our 2nd 2D environment
unsigned char           *out_buffer;        // 1D character array to represent our global 2D environment + padding

dist                     dist_type;

#else
extern int              nrows;   
extern int              ncols;   
extern int              my_row;  
extern int              my_col;  

extern int              local_width;    
extern int              local_height;
extern int              global_width;
extern int              global_height;
extern int              N;

extern int              field_width;
extern int              field_height;
extern unsigned char    *env_a;
extern unsigned char    *env_b;
extern unsigned char    *out_buffer;

extern dist              dist_type;

#endif
