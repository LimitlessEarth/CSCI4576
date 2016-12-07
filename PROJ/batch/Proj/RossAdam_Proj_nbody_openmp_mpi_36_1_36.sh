#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_36_1_36.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=36
#SBATCH --export=ALL  
#SBATCH -t 00:10:00

export OMP_NUM_THREADS=36
ibrun --npernode 2 nbody_openmp_mpi -i 60 -p 1000