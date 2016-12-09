#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_120_5_24_1024320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=5
#SBATCH --ntasks-per-node=24
#SBATCH --export=ALL  
#SBATCH -t 02:00:00

export OMP_NUM_THREADS=24
ibrun --npernode 1 nbody_openmp_mpi -i 10 -p 1024320