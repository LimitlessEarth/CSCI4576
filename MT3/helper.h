/* 
 * Helper function file to be included in main
 * Written by Adam Ross
 * 
 */

void print_usage();
void print_matrix(unsigned char *matrix);
void print_padded_matrix(unsigned char *matrix);
void print_global_matrix(unsigned char *matrix);
void swap(unsigned char **a, unsigned char **b);
unsigned char *Allocate_Square_Matrix();
int count_alive(unsigned char *matrix);
int Calc_Confidence_Interval_stop(double *timing_data, int n);