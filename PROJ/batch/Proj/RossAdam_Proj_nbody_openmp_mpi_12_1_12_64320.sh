#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_12_1_12_64320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#SBATCH --export=ALL  
#SBATCH -t 00:30:00

export OMP_NUM_THREADS=6
ibrun --npernode 2 nbody_openmp_mpi -i 25 -p 64320