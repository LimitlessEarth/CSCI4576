/* greetings.c -- greetings program
 *
 * printf on line 45 written by Adam Ross
 * 
 * Send a message from all processes with rank != 0 to process 0.
 *    Process 0 prints the messages received.
 *
 * Input: none.
 * Output: contents of messages received by process 0.
 *
 * From Chapter 3, pp. 41 & ff in PPMPI.
 */
#include <stdio.h>
#include <string.h>
#include <math.h> 
#include "mpi.h"

main(int argc, char* argv[]) {
    int         my_rank;       /* rank of process      */
    int         p;             /* number of processes  */
    int         source;        /* rank of sender       */
    int         mask;
    int         dest;          /* rank of receiver     */
    int         tag = 0;       /* tag for messages     */
    char        message[100];  /* storage for message  */
    MPI_Status  status;        /* return status for    */
                               /* receive              */
    

    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank  */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    //int a = Broadcast_lo(p, my_rank);
    int a = Broadcast_high(p, my_rank);
    //MPI_COMM_WORLD
    //int a = Reduce_lo(p, my_rank);
    //int a = Reduce_high(p, my_rank);

    /* Shut down MPI */
    MPI_Finalize();
} /* main */

int Broadcast_lo(int p, int my_rank) {
    int current_mask;
    int dest;
    for (int stage = 0; stage < log2(p); stage++) {
        current_mask = 1 << stage;
        if (my_rank < current_mask) {
            dest = my_rank | current_mask;
            printf("My rank is %d - Stage is: %d - Sending to %d\n", my_rank, stage, dest);
            // send            
        } //else {            
            // recv
        //}
    }

    return 1;
}

int Broadcast_high(int p, int my_rank) {
    int current_mask;
    int dest;
    for (int stage = 0; stage < log2(p); stage++) {
        current_mask = (p / 2) >> stage;
        dest = my_rank | current_mask;
        if ((my_rank % current_mask == 0) && (my_rank != dest)) {        
            printf("My rank is %d - Stage is: %d - Sending to %d\n", my_rank, stage, dest);
            // send
        } //else {
            // recv
        //}
    }

    return 1;
}

int Reduce_lo(int p, int my_rank) {
    int current_mask;
    int dest;
    for (int stage = 0; stage < log2(p); stage++) {
        current_mask = 1 << stage;
        if ((my_rank - current_mask) % ((stage + 1) * 2) == 0) {
            dest = my_rank ^ current_mask;
            printf("My rank is %d - Stage is: %d - Sending to %d\n", my_rank, stage, dest);
            // send
        } //else {
            // recv
        //}
    }

    return 1;
}

int Reduce_high(int p, int my_rank) {
    int current_mask;
    int dest;
    for (int stage = 0; stage < log2(p); stage++) {
        current_mask = (p / 2) >> stage;
        if ((my_rank >= current_mask) && (my_rank < current_mask * 2)) {
            dest = my_rank ^ current_mask;
            printf("My rank is %d - Stage is: %d - Sending to %d\n", my_rank, stage, dest);
            // send
        } //else {
            // recv
        //}
    }

    return 1;
}