#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_120_5_24_256320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=5
#SBATCH --ntasks-per-node=24
#SBATCH --export=ALL  
#SBATCH -t 00:50:00

export OMP_NUM_THREADS=24
ibrun --npernode 1 nbody_openmp_mpi -i 15 -p 256320