/* RossAdam_HW3.c
 *
 * Broadcast and Reduce methods
 * with high to low and low to high 
 * bit mask transversal.
 *
 * All_Reduce method compound from the above.
 * All_Reduce method using an all to one alogrithm.
 * 
 * Input: none.
 * Output: none.
 *
 * 
 */
#include <stdio.h>
#include <string.h>
#include <math.h> 
#include "mpi.h"
#include <getopt.h>
#include <stdlib.h>

double Butterfly_AllReduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank);
double My_Broadcast(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank);
double My_Reduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank);
double My_Compound_All_Reduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank);
double My_All_Reduce(double *buffer, int count, MPI_Comm comm, int p, int my_rank);

void print_usage() {
    printf("Usage: -program or -p to specify which program to run. -data-size or -d to specify data size in bytes.\n");
}

main(int argc, char* argv[]) {
    int         option = 0;
    int         my_rank;       /* rank of process      */
    int         p;             /* number of processes  */
    int         tag = 0;       /* tag for messages     */
    int         program = -1;  /* from argparse - number of program to run */
    int         data_size = 1; /* the number of byte of doubles to run on */
    double      result;
    
    while ((option = getopt(argc, argv,"P:d:")) != -1) {
        switch (option) {
             case 'P' : program = atoi(optarg); 
                 break;
             case 'd' : data_size = atoi(optarg);
                 break;
             default: print_usage(); 
                 exit(1);
        }
    }
    
    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank  */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    if (my_rank == 0) {
        printf("Number of program %d, number of datasize %d\n", program, data_size);
    }
    
    double      to_sum = 0;        /* storage for message  */
    double *sum_ptr = calloc(128, sizeof *sum_ptr);
    
    // DICK AROUND FOR A BIT TO INITALIZE MPI FULLY BEFORE BENCHMARKING
    //double warm_up = Butterfly_AllReduce(&to_sum, 1, MPI_COMM_WORLD, 0, p, my_rank);
    
    if (program == 0 || program == 1) {
        if (my_rank == 0) {
            to_sum = 3.0;
        }
    } else {
        to_sum = 3.0;
    }
    
    /* Comment or un comment to run specific programs */
    double start = MPI_Wtime();
    switch (program) {
        case 0 :
            result = My_Broadcast(&to_sum, data_size, MPI_COMM_WORLD, 1, p, my_rank);
            break;
        case 1 :
            result = My_Broadcast(&to_sum, data_size, MPI_COMM_WORLD, 0, p, my_rank);
            break;
        case 2 :
            result = My_Reduce(&to_sum, data_size, MPI_COMM_WORLD, 1, p, my_rank);
            break;
        case 3 :
            result = My_Reduce(&to_sum, data_size, MPI_COMM_WORLD, 0, p, my_rank);
            break;
        case 4 :
            result = My_Compound_All_Reduce(&to_sum, data_size, MPI_COMM_WORLD, 1, p, my_rank);
            break;
        case 5 :
            result = My_Compound_All_Reduce(&to_sum, data_size, MPI_COMM_WORLD, 0, p, my_rank);
            break;
        case 6 :
            result = My_All_Reduce(&to_sum, data_size, MPI_COMM_WORLD, p, my_rank);
            break;
        case 7 :
            result = Butterfly_AllReduce(&to_sum, data_size, MPI_COMM_WORLD, 1, p, my_rank);
            break;
        case 8 :
            result = Butterfly_AllReduce(&to_sum, data_size, MPI_COMM_WORLD, 0, p, my_rank);
            break;
        default:
            exit(1);
    }
    
    double finish = MPI_Wtime();
    
    double tick = MPI_Wtick();
    printf("Resolution of MPI wtime: %1.50f\nTotal time was: %1.50f\n", tick, (finish-start));
    

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
    printf("+ My rank is %d and the buffer contains %f\n", my_rank, *buffer);
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
    
    printf("+ My rank is %d and the buffer contains %f\n", my_rank, *buffer);
    
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

double My_All_Reduce(double *buffer, int count, MPI_Comm comm, int p, int my_rank) {
    int dest;
    int source;
    double total = 0;
    MPI_Status  status;
    
    // Reduce
    if (my_rank != 0) {
        dest = 0;
        printf("My rank is %d, sending to %d\n", my_rank, dest);
        MPI_Send(buffer, count + 1, MPI_DOUBLE, dest, 0, comm);
    } else {
        total = total + * buffer;
        for (int i = 1; i < p; i++) {
            source = i;
            printf("My rank is %d, receiving from %d\n", my_rank, source);
            MPI_Recv(buffer, count + 1, MPI_DOUBLE, source, 0, comm, &status);
            total = total + *buffer;
        }
        printf("- My rank is %d and the reduced value I have is %f\n",my_rank, total);
    }
    
    // Broadcast
    if (my_rank != 0) {
        source = 0;
        printf("My rank is %d, receiving from %d\n", my_rank, source);
        MPI_Recv(buffer, count + 1, MPI_DOUBLE, source, 0, comm, &status);
        total = *buffer;
    } else {
        for (int i = 1; i < p; i++) {
            printf("My rank is %d, sending to %d\n", my_rank, i);
            MPI_Send(&total, count + 1, MPI_DOUBLE, i, 0, comm);
        }
    }
    
    printf("+ My rank is %d, and the value I have is %f\n", my_rank, total);
    
    return total;
}

// (my_rank / mask) % 2 == 0; // add
double Butterfly_AllReduce(double *buffer, int count, MPI_Comm comm, int lo_hi, int p, int my_rank) {
    int current_mask;
    int dest_source;
    double total = *buffer;
    MPI_Status  status;
    
    printf("+ My rank is %d and the buffer contains %f\n", my_rank, *buffer);
    
    if (lo_hi != 0) { // hi
        for (int stage = 0; stage < log2(p); stage++) {
            current_mask = (p / 2) >> stage;
            if ((my_rank / current_mask) % 2 == 0) {
                dest_source = my_rank | current_mask;
                printf("+My rank is %d - Stage is: %d - Sending to then receiving from %d\n", my_rank, stage, dest_source);
                // send
                MPI_Send(&total, count + 1, MPI_DOUBLE, dest_source, 0, comm);
                MPI_Recv(buffer, count + 1, MPI_DOUBLE, dest_source, 0, comm, &status);
                total = total + *buffer;
            } else {
                // recv
                dest_source = my_rank ^ current_mask;
                printf("-My rank is %d - Stage is: %d - Receiving from and then sending to %d\n", my_rank, stage, dest_source);
                MPI_Recv(buffer, count + 1, MPI_DOUBLE, dest_source, 0, comm, &status);
                MPI_Send(&total, count + 1, MPI_DOUBLE, dest_source, 0, comm);
                total = total + *buffer;
            }
        }
    } else { // lo
        for (int stage = 0; stage < log2(p); stage++) {        
            current_mask = 1 << stage;
            if ((my_rank / current_mask) % 2 == 0) {
                dest_source = my_rank | current_mask;
                printf("My rank is %d - Stage is: %d - Sending to then receiving from %d\n", my_rank, stage, dest_source);
                // send
                MPI_Send(&total, count + 1, MPI_DOUBLE, dest_source, 0, comm); 
                MPI_Recv(buffer, count + 1, MPI_DOUBLE, dest_source, 0, comm, &status);
                total = total + *buffer;         
            } else {
                // recv
                dest_source = my_rank ^ current_mask;
                printf("My rank is %d - Stage is: %d - Receiving from and then sending to %d\n", my_rank, stage, dest_source);
                MPI_Recv(buffer, count + 1, MPI_DOUBLE, dest_source, 0, comm, &status);
                MPI_Send(&total, count + 1, MPI_DOUBLE, dest_source, 0, comm);
                total = total + *buffer;
            }
        }
    }
    printf("+ My rank is %d and the total is %f\n", my_rank, total);
    return total;
    
}