/* RossAdam_serial_simpson.c -- Serial Simpson's Rule
 *
 * 
 * Input: -i # or --intervals # = number of intervals
 * Output:  Estimate of the integral from a to b of f(x)
 *    using the trapezoidal rule and n trapezoids.
 *
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int n;    
    float a = 0.0;   /* Left endpoint             */
    float b = 1.0;   /* Right edndpoint            */
    float delta_x;
    float sum = 0.0;
    float current_x = 0.0;
    
    float f(float x);
    
    if (argc != 3) {
        printf("Too many or too few arguements. Expecting -i(or --intervals) #");
        return 1;
    } else if (!strcmp("-i", argv[1]) && !strcmp("--intervals", argv[1])) {
        return 1;
    }
    n = atoi(argv[2]);
    delta_x = (b - a) / n;
    
    sum += f(a);
    for (int i=1; i <= (n-1); i++) {
        current_x += delta_x;
        if (i % 2) { // odd
            sum += 4 * f(current_x);
        } else { // even
            sum += 2 * f(current_x);
        }
    }
    sum += f(b);
    
    sum = sum * (delta_x / 3);
    
    printf("of the integral from %f to %f = %f\n", a, b, sum); 
    
} /*  main  */

float f(float x) {
    float return_val;
    /* Calculate f(x). */
    /* Store calculation in return_val. */
    return_val = x*x;
    return return_val;
} /* f */