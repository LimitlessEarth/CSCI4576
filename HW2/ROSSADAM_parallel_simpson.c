/* RossAdam_parallel_simpson.c -- Parallel Simpson's Rule
 *
 * 
 * Input: -i # or --intervals # = number of intervals
 *        -v or --verbose = print additional information
 * Output:  Estimate of the integral from a to b of f(x)
 *    using the trapezoidal rule and n trapezoids.
 *
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* We'll be using MPI routines, definitions, etc. */
#include "mpi.h"

int main(int argc, char** argv) {
    int n;    
    float a = 0.0;   /* Left endpoint             */
    float b = 1.0;   /* Right edndpoint            */
    float delta_x;
    float sum = 0.0;
    
    int found;
    int indx;
    int verbose;
    
    int bins;
    int remain;
    int sum_a;
    
    int         my_rank;   /* My process rank           */
    int         p;         /* The number of processes   */
    float       total;     /* Total sum                 */
    int         source;    /* Process sending sum       */
    int         dest;      /* Message cascade to 0      */
    int         tag = 0;
    MPI_Status  status;
    
    /* Let the system do what it needs to start up MPI */
    MPI_Init(&argc, &argv);

    /* Get my process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out how many processes are being used */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    float f(float x);
    
    /* Pasre arguements and make sure our values work */
    if (argc != 3 && argc != 4) {
        if (my_rank == 0) {
            printf("Too many or too few arguements. Expecting -i(or --intervals) # and optionally -v(or --verbose)\n");
        }
        return 1;
    } 
    /* Check if we have -i or optionally if we have -v */
    found = 0;
    indx = 0;
    verbose = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp("-i", argv[i]) == 0 || strcmp("--intervals", argv[i]) == 0) {
            indx = i;
            found = 1;
        }
        if (strcmp("-v", argv[i]) == 0 || strcmp("--verbose", argv[i]) == 0) {
            verbose = 1;
        }
    }
    
    /* If we are not specified intervals */
    if (found == 0) {
        if (my_rank == 0) {
            printf("No intervals specified. Quitting.\n");
        }
        return 1;
    }
    
    /* set n to intervals specified */
    n = atoi(argv[indx + 1]);
    if (n <= 0){
        if (my_rank == 0) {
            printf("You cannot have negative or zero intervals.\n");
        }
        return 1;
    } else if (n % 2) { // Simpson's rule cannot operate with odd intervals
        if (my_rank == 0) {
            printf("An odd interval was given, which basic simpson's rule does not work with. Adding 1 interval.\n");
        }
        n++;
    }
    /* Done with house keeping */
    
    /* Individual process setup */
    bins = (int) (n / p);
    remain = n % p;
    
    /* Build coefficient array */
    int size_ca = (n + 1);
    int coef_array[size_ca];
    for (int i = 0; i < size_ca; i++) {
        if (i % 2) {
            coef_array[i] = 4;
        } else {
            coef_array[i] = 2;
        }
    }
    coef_array[0] = 1;
    coef_array[n] = 1;
    
    /* Build array of boundery coefficient index values for each process */
    int bound[p + 1];
    sum_a = 0;
    for (int i = 1; i < (p + 1); i++) {
        if (i < (remain + 1)) {
            sum_a += bins + 1;
            bound[i] = sum_a;
        } else {
            sum_a += bins;
            bound[i] = sum_a;
        }
    }
    bound[0] = 0;
    bound[p] += 1;
    
    /* If we have been passed -v print information */
    if (verbose && my_rank == 0) {
        for (int i = 0; i < p; i++) {
            printf("[ ");
            for (int j = bound[i]; j < bound[i+1]; j++) {
                printf("%d ", coef_array[j]);
            }
            printf("] ");
        }
        printf("\n");   
    }
    
    /* Do the actual Simpson's rule work */
    delta_x = (b - a) / n;
    for (int i = bound[my_rank]; i < bound[my_rank + 1]; i++) {
        sum += (coef_array[i] * f(i * delta_x));
    }
    
    /* Fold the data back down to process 0 */
    source = my_rank + 1;
    dest = (my_rank == 1 ? 0 : (my_rank - 1));
    if (p != 1) {
        if (my_rank == (p - 1)) {
            MPI_Send(&sum, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
        } else {
            MPI_Recv(&total, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
            total = total + sum;
            if (dest != -1) {
                MPI_Send(&total, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
            }
        }
    } else {
        total = sum;
    }
    
    /* Do the final Simpson's rule multiplication and print*/
    if (my_rank == 0) {
        total = total * (delta_x / 3.0);
        printf("The parallel sum for Simpson's rule from %f to %f is %f\n", a, b, total);
    }

    /* Shut down MPI */
    MPI_Finalize();
    
} /*  main  */

float f(float x) {
    float return_val;
    /* Calculate f(x). */
    /* Store calculation in return_val. */
    return_val = x*x;
    return return_val;
} /* f */