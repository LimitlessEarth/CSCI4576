#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_12_1_12_32160.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#SBATCH --export=ALL  
#SBATCH -t 00:25:00

export OMP_NUM_THREADS=6
ibrun --npernode 2 nbody_openmp_mpi -i 30 -p 32160