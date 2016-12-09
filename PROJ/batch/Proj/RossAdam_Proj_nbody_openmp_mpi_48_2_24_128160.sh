#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_48_2_24_128160_4_6.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=24
#SBATCH --export=ALL  
#SBATCH -t 00:45:00

export OMP_NUM_THREADS=6
ibrun --npernode 4 nbody_openmp_mpi -i 20 -p 128160