#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_mpi_60_3_20_480.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=20
#SBATCH --export=ALL  
#SBATCH -t 00:5:00

export OMP_NUM_THREADS=20
ibrun --npernode 1 nbody_openmp_mpi -i 100 -p 480