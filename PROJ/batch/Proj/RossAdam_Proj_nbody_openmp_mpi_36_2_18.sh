#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_36_2_18.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=18
#SBATCH --export=ALL  
#SBATCH -t 00:10:00

export OMP_NUM_THREADS=18
ibrun --npernode 1 nbody_openmp_mpi -i 60 -p 1000