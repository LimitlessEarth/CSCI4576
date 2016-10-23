#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include <math.h>

// Self explanitory
void print_usage() {
    printf("Usage: -i filename, -d distribution type <0 - serial, 1 - row, 2 - grid>, -s turn on asynchronous MPI functions, -c <#> if and when to count living\n");
}

/* 
 * Helper method to print a square matrix
 * Input: a matrix and the order of that matrix
 */
void print_matrix(unsigned char *matrix) {
    unsigned char          i;
    unsigned char          j;
    
    //printf("local_width is: %d, local_height is: %d\n", local_width, local_height);
    
    for (i = 1; i < local_height + 1; i++) {
        for (j = 1; j < local_width + 1; j++) {
            printf("%u ", matrix[i * field_width + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_padded_matrix(unsigned char *matrix) {
    unsigned char          i;
    unsigned char          j;
    
    //printf("local_width is: %d, local_height is: %d\n", local_width, local_height);
    
    for (i = 0; i < field_height; i++) {
        for (j = 0; j < field_width; j++) {
            printf("%u ", matrix[i * field_width + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_global_matrix(unsigned char *matrix) {
    unsigned char          i;
    unsigned char          j;
    
    //printf("local_width is: %d, local_height is: %d\n", local_width, local_height);
    
    for (i = 0; i < global_height; i++) {
        for (j = 0; j < global_width; j++) {
            printf("%u ", matrix[i * global_width + j]);
        }
        printf("\n");
    }
    printf("\n");
}

/*
 * Helper function to swap array pointers
 * Input: array a and Array b
 */
void swap(unsigned char **a, unsigned char **b) {
    unsigned char          *tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * Helper function to allocate 2D array of ints
 * Input: Order of the array
 */
unsigned char *Allocate_Square_Matrix(int width, int height) {
    unsigned char          *matrix;
    
    matrix = (unsigned char *) calloc(width * height, sizeof(unsigned char));
    
    return matrix;
}

/*
 * Helper function to clean up code duplication
 * Input: pointer to array
 */
int count_alive(unsigned char *matrix) {
    int                   count =           0;
    int                   i, j;
    
    for (i = 1; i < local_height + 1; i++) {
        for (j = 1; j < local_width + 1; j++) {
            if (matrix[i * field_width + j]) {
                count ++;
            }
        }
    }
    
    return count;
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