#include <stdio.h>
#include <math.h>

int main() {
    float num1;
    float num2;

    scanf("%f %f", &num1, &num2);
    printf("%*d \n", 20, (int)num1);
    printf("%*d \n", 20, (int)num2);
}
