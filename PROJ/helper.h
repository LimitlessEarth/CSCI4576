/* 
 * Helper function file to be included in main
 * Written by Adam Ross
 * 
 */

void print_usage();
void swap(Particle **a, Particle **b);
void swap_pass(Pass_Particle **a, Pass_Particle **b);
void globals_init();
void initialize_particles();
void set_pass_particles();
void write_data_serial(int n);
int Calc_Confidence_Interval_stop(double *timing_data, int n);