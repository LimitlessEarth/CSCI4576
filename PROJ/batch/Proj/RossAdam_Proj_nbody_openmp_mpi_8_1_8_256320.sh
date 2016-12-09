#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_8_1_8_256320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --export=ALL  
#SBATCH -t 01:00:00

export OMP_NUM_THREADS=4
ibrun --npernode 2 nbody_openmp_mpi -i 15 -p 256320