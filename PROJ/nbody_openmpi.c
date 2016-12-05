#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

// Include global variables. Only this file needs the #define
#define __MAIN 
#include "globals.h"
#undef __MAIN

// User includes
#include "pprintf.h"
#include "helper.h"

int main (int argc, char** argv) {
    double              start, end, start_writing, end_writing, start_tot, end_tot;    
    double              dx, dy, dz, ax, ay, az, a, dist;
    //MPI_Request         Request;
    MPI_Datatype        MPI_Particle, MPI_Particle_Full;
    MPI_Status          status;
    MPI_Datatype        darray;
    MPI_File            out_file;
        
    int                 gsizes[2], distribs[2], dargs[2], psizes[2];
    int                 i, j, k, x, y, b, frame, source, dest;
    short                 *out_buffer_sc;
    char                frame_name[47]; 
    
    globals_init();
    parse_args(argc, argv);
    
    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get the communicator and process information
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // Print rank and hostname
    MPI_Get_processor_name(my_name, &my_name_len);
    pprintf("Rank %i is running on %s\n", rank, my_name );
    
    // Initialize the pretty printer
    init_pprintf(rank);
    
    my_num_part = num_part / np;
    source = (rank - 1 + np) % np;
    dest = (rank + 1) % np;
    
    pprintf("My dest: %d\tMy source: %d\tMy_num_part: %d\n", dest, source, my_num_part);
    
    if (num_part % np != 0) {
        MPI_Finalize();
        printf("Cannot divide particles among processes evenly. My_num_part: %d\t Mod: %d\n", my_num_part, my_num_part % np);
        return 1;
    }
    
    // Create a particle with only the passing data
    MPI_Type_contiguous(NDIM + 1, MPI_DOUBLE, &MPI_Particle);
    MPI_Type_commit(&MPI_Particle);
    
    // Create particle that contains all the data necessary
    MPI_Type_contiguous(2 * NDIM + 1, MPI_DOUBLE, &MPI_Particle_Full);
    MPI_Type_commit(&MPI_Particle_Full);
    
    // Set up darray create properties
    gsizes[0] = my_num_part; /* no. of rows in global array */
    gsizes[1] = 1; /* no. of columns in global array*/
    distribs[0] = MPI_DISTRIBUTE_BLOCK;
    distribs[1] = MPI_DISTRIBUTE_BLOCK;
    dargs[0] = MPI_DISTRIBUTE_DFLT_DARG;
    dargs[1] = MPI_DISTRIBUTE_DFLT_DARG;
    psizes[0] = 1; /* no. of processes in vertical dimension of process grid */
    psizes[1] = np; /* no. of processes in horizontal dimension of process grid */
    
    // Create darray
    MPI_Type_create_darray(np, rank, 2, gsizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_SHORT, &darray);
    MPI_Type_commit(&darray);

    Particles_a = (Particle *) malloc(my_num_part * sizeof(Particle));
    Particles_b = (Particle *) malloc(my_num_part * sizeof(Particle));
    Particles_pass_a = (Pass_Particle *) malloc(my_num_part * sizeof(Pass_Particle));
    Particles_pass_b = (Pass_Particle *) malloc(my_num_part * sizeof(Pass_Particle));
    
    if (writing) {
        if (super_comp) {
            out_buffer_sc = (short *) calloc(my_num_part * 2, sizeof(short));
        } else {
            Particles_out = (Particle *) malloc(num_part * sizeof(Particle));
            out_buffer = (char *) calloc(img_len, sizeof(char));
        }
    }
    
    initialize_particles();
        
    start_tot = MPI_Wtime();
    
    for (frame = 0; frame < num_iter; frame++) {

        start_writing = MPI_Wtime();  
        
        if (writing) {
            if (super_comp) {
                for (b = 0; b < my_num_part * 2; b = b + 2) {        
                    x = (Particles_out[b].pos[X] / DOMAIN_SIZE) * img_dim;        
                    y = (Particles_out[b].pos[Y] / DOMAIN_SIZE) * img_dim;
                
                    out_buffer_sc[b] = x;
                    out_buffer_sc[b + 1] = y;
        
                }
       
                sprintf(frame_name, "/oasis/scratch/comet/adamross/temp_project/%d.dat", frame);
                MPI_File_open(MPI_COMM_WORLD, frame_name, MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &out_file);

                // write data
                MPI_File_set_view(out_file, 0, MPI_SHORT, darray, "native", MPI_INFO_NULL);

                MPI_File_write(out_file, out_buffer_sc, my_num_part * 2, MPI_SHORT, &status);
                MPI_File_close(&out_file);
    
            } else {
                MPI_Gather(Particles_a, my_num_part, MPI_Particle_Full, 
                            Particles_out, my_num_part, MPI_Particle_Full, 0, MPI_COMM_WORLD);
          
                if (rank == 0) {
                    write_data_parallel(frame);
                }
            }
        }

        end_writing = MPI_Wtime(); 

        start = MPI_Wtime();
        
        // Set this frame's pass particle values My_Pass_Part = My_Part
        set_pass_particles();

        // for all MPI processes
        for (k = 0; k < np; k++) {
            #pragma omp parallel for schedule(static) private(dist, dx, dy, dz, a, ax, ay, az, i, j) shared(frame, Particles_a, Particles_b, dt)
            for (i = 0; i < my_num_part; i++) { // for particle i
                // acceleration for particle i
                ax = 0, ay = 0, az = 0;
                for (j = 0; j < my_num_part; j++) { // calculate force based on all other particles
                    //   Particles passing       My particles
                    dx = Particles_pass_a[j].pos[X] - Particles_a[i].pos[X];
                    dy = Particles_pass_a[j].pos[Y] - Particles_a[i].pos[Y];
                    dz = Particles_pass_a[j].pos[Z] - Particles_a[i].pos[Z];
                
                    dist = sqrt(dx * dx + dy * dy + dz * dz) + 1;
                
                    if (dist > DOMAIN_SIZE) {
                        continue;
                    }
                
                    // Passing Particle
                    a = (G * Particles_pass_a[j].mass) / (dist * dist * EPS);
                
                    /* accumulate the acceleration from gravitational attraction */
                    ax += a * dx;
                    ay += a * dy;
                    az += a * dz;
                
                    /* update velocity of particle "i" */
                    Particles_a[i].vel[X] += dt * ax;
                    Particles_a[i].vel[Y] += dt * ay;
                    Particles_a[i].vel[Z] += dt * az;
                }
            
                /*
                *  Here we accumulate the x, y, z velocities and calculaye the next frame
                */
            
                /* update position of particle "i" */
                Particles_b[i].pos[X] = Particles_a[i].pos[X] + dt * Particles_a[i].vel[X];
                Particles_b[i].pos[Y] = Particles_a[i].pos[Y] + dt * Particles_a[i].vel[Y];
                Particles_b[i].pos[Z] = Particles_a[i].pos[Z] + dt * Particles_a[i].vel[Z];
            
            } 
            
            //Communication
            MPI_Sendrecv_replace(Particles_pass_a, my_num_part, MPI_Particle, dest, 0, source, 0, MPI_COMM_WORLD, &status);
                         
            //MPI_Irecv(Particles_pass_b, my_num_part, MPI_Particle, source, 0, MPI_COMM_WORLD, &Request);
            //MPI_Isend(Particles_pass_a, my_num_part, MPI_Particle, dest, 0, MPI_COMM_WORLD, &Request);
            
            //MPI_Wait(&Request, &status);
            
            //swap_pass(&Particles_pass_a, &Particles_pass_b);
        }
        
        end = MPI_Wtime();
        
        printf("%f\t%f\n", end-start, end_writing-start_writing);
        
        swap(&Particles_a, &Particles_b);
    }
    
    end_tot = MPI_Wtime();
    
    if (rank == 0) {
        printf("Total computation time was: %f", end_tot - start_tot);
    }
    
    MPI_Finalize();
    
}