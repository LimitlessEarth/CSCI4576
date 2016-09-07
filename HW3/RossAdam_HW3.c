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

double My_Broadcast(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank);
double My_Reduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank);
double My_Compound_All_Reduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank);
double My_All_Reduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank);


main(int argc, char* argv[]) {
    int         my_rank;       /* rank of process      */
    int         p;             /* number of processes  */
    int         tag = 0;       /* tag for messages     */
    double      to_sum;        /* storage for message  */
    
    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank  */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    //MPI_COMM_WORLD
    //if (my_rank == 0) {
    to_sum = 3.0;
    //}
    
    //double a = My_Broadcast(&to_sum, 1, MPI_COMM_WORLD, 1, p, my_rank);
    //double b = My_Reduce(&to_sum, 1, MPI_COMM_WORLD, 0, p, my_rank);
    //double c = My_Compound_All_Reduce(&to_sum, 1, MPI_COMM_WORLD, 0, p, my_rank);
    double d = My_All_Reduce(&to_sum, 1, MPI_COMM_WORLD, 0, p, my_rank);

    /* Shut down MPI */
    MPI_Finalize();
} /* main */


// low to high bit transversal
// 001 -> 010 -> 100
//  1      2      4

// hight to low bit transversal
// 100 -> 010 -> 001
//  4      2      1

double My_Broadcast(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank) {
    int current_mask;
    int dest;
    int source;
    MPI_Status  status;
    //printf("+ My rank is %d and the buffer contains %f\n", my_rank, *buffer);
    if (lo_hi != 0) { // hi
        for (int stage = 0; stage < log2(p); stage++) {
            current_mask = (p / 2) >> stage;
            dest = my_rank | current_mask;
            if ((my_rank % current_mask == 0) && (my_rank != dest)) {        
                printf("My rank is %d - Stage is: %d - Sending to %d\n", my_rank, stage, dest);
                //send
                MPI_Send(buffer, count + 1, MPI_DOUBLE, dest, 0, comm);          
            } else if ((my_rank + current_mask) % (2 * current_mask) == 0) {
                //recv
                source = my_rank ^ current_mask;
                printf("My rank is %d - Stage is: %d - Receiving from %d\n", my_rank, stage, source);
                MPI_Recv(buffer, count + 1, MPI_DOUBLE, source, 0, comm, &status);
            }
        }
    } else { // lo
        for (int stage = 0; stage < log2(p); stage++) {
            current_mask = 1 << stage;
            if (my_rank < current_mask) {
                dest = my_rank | current_mask;
                printf("My rank is %d - Stage is: %d - Sending to %d\n", my_rank, stage, dest);
                // send
                MPI_Send(buffer, count + 1, MPI_DOUBLE, dest, 0, comm);          
            } else if ((my_rank >= current_mask) && (my_rank < (current_mask * 2))) {            
                // recv
                source = my_rank - current_mask;
                printf("My rank is %d - Stage is: %d - Receiving from %d\n", my_rank, stage, source);
                MPI_Recv(buffer, count + 1, MPI_DOUBLE, source, 0, comm, &status);
            }
        }
    }
    printf("+ My rank is %d and the buffer contained %f\n", my_rank, *buffer);
    return *buffer;
}

double My_Reduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank) {
    int current_mask;
    int dest;
    int source;
    double total = *buffer;
    MPI_Status  status;
    
    //printf("+ My rank is %d and the buffer contains %f\n", my_rank, *buffer);
    
    if (lo_hi != 0) { // hi
        for (int stage = 0; stage < log2(p); stage++) {
            current_mask = (p / 2) >> stage;
            if ((my_rank >= current_mask) && (my_rank < current_mask * 2)) {
                dest = my_rank ^ current_mask;
                printf("My rank is %d - Stage is: %d - Sending to %d\n", my_rank, stage, dest);
                // send
                MPI_Send(&total, count + 1, MPI_DOUBLE, dest, 0, comm);          
            } else if (my_rank < current_mask) {
                // recv
                source = my_rank + current_mask;
                printf("My rank is %d - Stage is: %d - Receiving from %d\n", my_rank, stage, source);
                MPI_Recv(buffer, count + 1, MPI_DOUBLE, source, 0, comm, &status);
                total = total + *buffer;
            }
        }
    } else { // lo
        for (int stage = 0; stage < log2(p); stage++) {        
            current_mask = 1 << stage;
            if ((my_rank - current_mask) % ((stage + 1) * 2) == 0) {
                dest = my_rank ^ current_mask;
                printf("My rank is %d - Stage is: %d - Sending to %d\n", my_rank, stage, dest);
                // send
                MPI_Send(&total, count + 1, MPI_DOUBLE, dest, 0, comm);          
            } else if ((my_rank % (current_mask * 2)) == 0) {
                // recv
                source = my_rank + current_mask;
                printf("My rank is %d - Stage is: %d - Receiving from %d\n", my_rank, stage, source);
                MPI_Recv(buffer, count + 1, MPI_DOUBLE, source, 0, comm, &status);
                total = total + *buffer;
            }
        }
    }
    printf("+ My rank is %d and the total is %f\n", my_rank, total);
    return total;
    
}

double My_Compound_All_Reduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank) {
    double red = My_Reduce(buffer, count, comm, lo_hi, p, my_rank);
    double a =  My_Broadcast(&red, count, comm, lo_hi, p, my_rank);
    printf("- MY rank is %d, and the number I have is %f\n", my_rank, a);
    return a;
}

// That is, have every processor send to one processor, which receives using a loop, and then broadcast to each using MPI_Send from a loop

double My_All_Reduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank) {
    int dest;
    int source;
    double total = 0;
    MPI_Status  status;
    
    // Reduce
    if (my_rank != 0) {
        dest = 0;
        //printf("My rank is %d, sending to %d\n", my_rank, dest);
        MPI_Send(buffer, count + 1, MPI_DOUBLE, dest, 0, comm);
    } else {
        total = total + * buffer;
        for (int i = 1; i < p; i++) {
            source = i;
            //printf("My rank is %d, receiving from %d\n", my_rank, source);
            MPI_Recv(buffer, count + 1, MPI_DOUBLE, source, 0, comm, &status);
            total = total + *buffer;
        }
        printf("- My rank is %d and the reduced value I have is %f\n",my_rank, total);
    }
    
    // Broadcast
    if (my_rank != 0) {
        source = 0;
        //printf("My rank is %d, receiving from %d\n", my_rank, source);
        MPI_Recv(buffer, count + 1, MPI_DOUBLE, source, 0, comm, &status);
        total = *buffer;
    } else {
        for (int i = 1; i < p; i++) {
            //printf("My rank is %d, sending to %d\n", my_rank, i);
            MPI_Send(&total, count + 1, MPI_DOUBLE, i, 0, comm);
        }
    }
    
    printf("+ My rank is %d, and the value I have is %f\n", my_rank, total);
    
    return total;
}

void Print(char* message, int my_rank) {
    printf("My rank is %d: %s\n",my_rank, message);
}