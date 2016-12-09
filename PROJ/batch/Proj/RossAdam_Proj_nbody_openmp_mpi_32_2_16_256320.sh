#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_32_2_16_256320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=16
#SBATCH --export=ALL  
#SBATCH -t 01:00:00

export OMP_NUM_THREADS=16
ibrun --npernode 1 nbody_openmp_mpi -i 15 -p 256320